import sys
import smbus2 as smbus#,smbus2
import time
from pymavlink import mavutil
import navpy

LATITUDE_TARGET = 0
LONGITUDE_TARGET = 0
YAW_NORTH_BIAS = 0

I2Cbus = smbus.SMBus(1)
slaveAddress = 0x08

data = [0,0,0,0,0,0]
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
   
    NED_target = navpy.lla2ned(LATITUDE_TARGET, LONGITUDE_TARGET, 0, latitude, longitude, 0)
    # Wait for an ATTITUDE message (yaw data)
    attitude_msg = connection.recv_match(type='ATTITUDE', blocking=True)
    # print('hi')
    # Extract yaw (heading vector) in radians
    yaw = attitude_msg.yaw

    # Convert yaw from radians to degrees
    yaw_degrees = yaw * 180 / 3.14159265359 + 180 + YAW_NORTH_BIAS
    # Wait for a VFR_HUD message (velocity data)
    velocity_msg = connection.recv_match(type='VFR_HUD', blocking=True)
    
    # Extract velocity data
    ground_speed = velocity_msg.groundspeed  # Ground speed in meters per second

    NED_target[0] = (NED_target[0]*10)%1
    NED_target[1] = (NED_target[1]*10)%1
    yaw_degrees = (yaw_degrees*10)%1
    data[0] = (NED_target[0]/256)%1
    data[1] = NED_target[0] % 256
    data[2] = (NED_target[1]/256)%1
    data[3] = NED_target[1] % 256
    data[4] = (yaw_degrees/256)%1
    data[5] = yaw_degrees % 256
    data.astype(int)
    with smbus.SMBus(1) as I2Cbus:
        I2Cbus.write_i2c_block_data(slaveAddress, 0x00, data)
        time.sleep(0.1)

    print(f"Latitude: {latitude}, Longitude: {longitude}, Altitude: {altitude} m, Yaw: {yaw_degrees} degrees, Ground Speed: {ground_speed} m/s")
   # print(f" Yaw: {yaw_degrees}")
    # time.sleep(1)  # Update GPS data, yaw, and velocity every second
