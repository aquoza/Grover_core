/** Arduino code for GRover_V2

NOTES:
   
1)[  ] Set correct reciver channels [21 - 25]
2)[  ] Test if inverse mapping is fine
3)[  ] check if switches mapping are correct (SwitchC)
4)[  ] check target heading float int conversion
5)[  ] check correct heading if its oscillating
**/

#include <Arduino.h>

// Define the UART parameters
const int uartTxPin = 17;  // GPIO pin for UART TX
const int uartRxPin = 16;  // GPIO pin for UART RX
const int baudRate = 19200;

// Define Input Connections
#define CH1 3
#define CH2 5
#define CH3 6
#define CH4 9
#define CH5 10
#define CH6 11

// Actions
uint8_t IDLE[] = {1, 0, 90, 0};
uint8_t LEFT[] = {3, 100, 0, 2};
uint8_t RIGHT[] = {3, 100, 2, 0};
uint8_t FORWARD[] = {1, 100, };

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

	speed = map(throttle, 950, 2050, -255 , 255);
	if(abs(speed) < 20){speed = 0;}
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
			if(120 < modifier && modifier < 134){modifier = 127;}
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


uint8_t* correct_heading(double current_heading, double target_heading){
	if (current_heading > target_heading){
		return LEFT;
	}
	else{
		return RIGHT;
	}
}

uint8_t* Autonomous (vector<double>target_GPS, vector<double> current_GPS, double current_heading){

	//Implement queue for moving average
	queue <vector<double>> avg_Q;
	vector<double> moving_average = 0;

	if(moving_average.size() < 11){
		avg_Q.push(current_GPS);
		moving_average[0] += current_GPS[0];
		moving_average[1] += current_GPS[1];  
		return IDLE;
	}

	moving_average[0] -= avg_Q.front()[0];
	moving_average[1] -= avg_Q.front()[1];
	avg_Q.pop();
	avg_Q.push(current_GPS);
	moving_average[0] += avg_Q.end()[0];
	moving_average[1] += avg_Q.end()[1];

	current_GPS[0] = moving_average[0]/10;
	current_GPS[1] = moving_average[1]/10;

	//convert target heading to degrees
	double target_heading = 90 - atan((target_GPS[0] - current_GPS[0])/
										(target_GPS[1] - current_GPS[1]))*57.2957795131;

	//correct heading if its off
	if(!(target_heading*0.93 < current_heading && current_heading < target_heading*1.07)){
		return correct_heading;
	}
	
	//Is reached
	if(target_GPS[0] - 2 < current_GPS[0] && current_GPS[1] < target_GPS[0] + 2 &&
		target_GPS[1] - 2 < current_GPS[1] && current_GPS[1] < target_GPS[1] + 2){
		return IDLE;
	}

	return FORWARD;

}

void setup() {
	delay(5000);
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
	if(SwitchC > 1500){
		command = Autonomous(GPS, heading);
	}
	else {
		command = Manual(mode, throttle, steering);
	}

	// Send the array over UART
	int arraySize = 4;
	Serial.write(command, sizeof(int) * arraySize);

	delay(100);  // Adjust the delay based on your requirements

}
