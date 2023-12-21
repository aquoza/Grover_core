/** Arduino code for GRover_V2 **/

#include <Arduino.h>
#include <Wire.h>
#include "params.h"

/**
  0 - targetgps_NHIGH
  1 - targetgps_NLOW
  2 - targetgps_EHIGH
  3 - targetgps_ELOW
  4 - currentgps_NHIGH
  5 - currentgps_NLOW
  6 - currentgps_EHIGH
  7 - currentgps_ELOW
  8 - currentheadingHIGH
  9 - currentheadingLOW
 **/
byte dataGet[6]; //input data from RPI
double target_GPS[2];
double current_heading;
int count = 0;
double moving_average[3] = {0,0,0};
double head[3] = {0,0,0};
double tail[3] = {MOVNG_AVERAGE_SIZE - 1, MOVNG_AVERAGE_SIZE - 1, MOVNG_AVERAGE_SIZE - 1};
double N_array[MOVNG_AVERAGE_SIZE];
double E_array[MOVNG_AVERAGE_SIZE];
double yaw_array[MOVNG_AVERAGE_SIZE];

void recieveData(int byteCount)
{
	while(Wire.available()){
		for (int i = 0; i < 6; i++) {
			dataGet[i] = Wire.read(); 
		}
		target_GPS[0] = (dataGet[0] << 8 | dataGet[1])/10;
		target_GPS[1] = (dataGet[2] << 8 | dataGet[3])/10;
		current_heading = (dataGet[4] << 8 | dataGet[5])/10;
	}
}

/*
   Manual RC control
INPUTS :
- mode : rover operation mode
- throttle : raw throttle reading
- steering : raw steering reading

OUTPUTS :
- array with four STM parameters
 */
uint8_t* Manual(uint8_t mode, int throttle, int steering){

	uint8_t output[] = {1 , 0 , 90 , 0};
	uint8_t speed,modifier;
	uint8_t direction = 0;

	speed = map(throttle, 950, 2050, -MAX_SPEED , MAX_SPEED);
	if(abs(speed) < ERR_SPEED){speed = 0;}  //speed err
	else if(speed < 0){
		speed =abs(speed);
		direction = 2;
	}

	switch (mode){

		//Strafe
		case 1 : 
			modifier = map(steering, 950, 2050, 0, 180);
			if(80 < modifier && modifier < 100){modifier = 90;}
			break;

			//Ackermann
		case 2 : 
			modifier = map(steering, 950, 2050, 0 , 255);
			if(126 - ERR_ACKERMANN < modifier && modifier < 126 + ERR_ACKERMANN){modifier = 127;}
			break;

			//UTurn
		case 3 : 
			if(direction == 2){ modifier = 0; }
			else if(speed == 0){ modifier = 1; }
			else{modifier = 2;}
			break;

			//Idle
		case 0 :
			mode = 1;
			speed = 0;
			modifier = 90;
			direction = 0;
			break;

		default :
			mode = 1;
			speed = 0;
			modifier = 90;
			direction = 0;
			break;
	}

	output[0] = mode;
	output[1] = speed;
	output[2] = modifier;
	output[3] = direction;

	return output;
}

double calculate_heading(double target_GPS){
	if(target_GPS[1] >= 0){
		return 90 - atan(target_GPS[0]/target_GPS[1])*57.2957795131;
	}
	else{
		return -90 - atan(target_GPS[0]/target_GPS[1])*57.2957795131;
	}
}

uint8_t* correct_heading(double current_heading, double target_heading){

	if (current_heading > target_heading){
		return LEFT;
	}
	else{
		return RIGHT;
	}
}

uint8_t* Autonomous (double target_GPS[2],  double current_heading){

	// Implement queue for moving average
	if(count < MOVNG_AVERAGE_SIZE){
		N_array[count] = target_GPS[0];
		E_array[count] = target_GPS[1];
		yaw_array[count] = current_heading;

		moving_average[0] += target_GPS[0];
		moving_average[1] += target_GPS[1];
		moving_average[2] += current_heading;
		count ++;
		return IDLE;
	}
		
	moving_average[0] += target_GPS[0] - N_array[head[0]];
	moving_average[1] += target_GPS[1] - E_array[head[0]];
	moving_average[2] += current_heading - yaw_array[head[2]];

	for(int i = 0; i < 3; i++){
		head[i] = (head[i] + 1)%MOVNG_AVERAGE_SIZE;
		tail[i] = (tail[i] + 1)%MOVNG_AVERAGE_SIZE;
	}

	N_array[tail[0]] = target_GPS[0];
	E_array[tail[1]] = target_GPS[1];
	yaw_array[tail[2]] = current_heading;

	target_GPS[0] = moving_average[0]/MOVNG_AVERAGE_SIZE;
	target_GPS[1] = moving_average[1]/MOVNG_AVERAGE_SIZE;
	current_heading = moving_average[2]/MOVNG_AVERAGE_SIZE;

	//convert target heading to degrees
	double target_heading = calculate_heading(target_GPS);

	//Is reached
	if(-2 < target_GPS[0] && target_GPS[0] < 2 && -2 < target_GPS[1] && target_GPS[1] < 2){
		return IDLE;
	}
	//correct heading if its off
	if(!(target_heading - ERR_HEADING < current_heading && current_heading < target_heading + ERR_HEADING)){
		return correct_heading(current_heading, target_heading);
	}


	return FORWARD;

}

void setup() {
	delay(5000);

	Wire.begin(0x08);
	Wire.onReceive(recieveData);

	Serial.begin(baudRate);

	pinMode(CH1, INPUT);
	pinMode(CH2, INPUT);
	pinMode(CH3, INPUT);
	pinMode(CH4, INPUT);
	pinMode(CH5, INPUT);
	pinMode(CH6, INPUT);
}

void loop(){

	// Get readings
	int throttle = pulseIn(CH1, HIGH, 30000);
	int steering = pulseIn(CH2, HIGH, 30000);
	int SwitchA = pulseIn(CH3, HIGH, 30000);
	int SwitchB = pulseIn(CH4, HIGH, 30000);
	int SwitchC = pulseIn(CH5, HIGH, 30000);


	int mode = 0;
	//Getting mode
	if(SwitchA == 0 && SwitchB == 0){ mode = 0; }
	else if (SwitchA == 1 && SwitchB == 0){ mode = 1; }
	else if (SwitchA == 0 && SwitchB == 1){ mode = 2; }
	else { mode = 3; }

	//Autonomous or Manual depending on SwitchC
	int arraySize = 4;
	if(SwitchC > 1500){
		Serial.write(Autonomous(target_GPS, current_GPS, current_heading), sizeof(int) * arraySize);
	}
	else {
		Serial.write(Manual(mode, throttle, steering), sizeof(int) * arraySize);
		delay(100);  // Adjust the delay based on your requirements
	}

}
