import time
from pymavlink import mavutil

# Connect to Pixhawk over the appropriate connection (e.g., '/dev/ttyUSB0' or 'udpin:0.0.0.0:14550')
connection = mavutil.mavlink_connection('/dev/ttyACM0')
print(connection)
while True:
    # Wait for a GLOBAL_POSITION_INT message (GPS data)
    gps_msg = connection.recv_match(type='GLOBAL_POSITION_INT', blocking=True)
    
    # # Extract GPS coordinates
    latitude = gps_msg.lat / 1e7  # Convert from degrees * 1e7 to degrees
    longitude = gps_msg.lon / 1e7  # Convert from degrees * 1e7 to degrees
    altitude = gps_msg.alt / 1000  # Convert from millimeters to meters
    
    # Wait for an ATTITUDE message (yaw data)
    attitude_msg = connection.recv_match(type='ATTITUDE', blocking=True)
    print('hi')
    # Extract yaw (heading vector) in radians
    yaw = attitude_msg.yaw

    # Convert yaw from radians to degrees
    yaw_degrees = yaw * 180 / 3.14159265359

    # Wait for a VFR_HUD message (velocity data)
    velocity_msg = connection.recv_match(type='VFR_HUD', blocking=True)
    
    # Extract velocity data
    ground_speed = velocity_msg.groundspeed  # Ground speed in meters per second

    print(f"Latitude: {latitude}, Longitude: {longitude}, Altitude: {altitude} m, Yaw: {yaw_degrees} degrees, Ground Speed: {ground_speed} m/s")
   # print(f" Yaw: {yaw_degrees}")
    # time.sleep(1)  # Update GPS data, yaw, and velocity every second
