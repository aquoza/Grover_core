#include <Arduino.h>

int MAX_SPEED = 250; //0 - 255
int ERR_SPEED = 20; //0 - 255
int ERR_ACKERMANN = 6; // 0 - 255
int ERR_HEADING = 30; // in degrees
int MOVNG_AVERAGE_SIZE = 10;


double target_GPS[2];
double current_heading;
int count = 0;
double moving_average[3] = {0,0,0};
int head[3] = {0,0,0};
int tail[3] = {MOVNG_AVERAGE_SIZE - 1, MOVNG_AVERAGE_SIZE - 1, MOVNG_AVERAGE_SIZE - 1};
double N_array[10];
double E_array[10];
double yaw_array[10];
uint8_t output_M[4] = {1 , 0 , 90 , 1};
// uint8_t output_A[4] = {1 , 0 , 90 , 1};

int state_current = 0;
int state_next = 0;

// Actions
uint8_t IDLE[] = {1, 0, 90, 1};
uint8_t LEFT[] = {3, 200, 0, 2};
uint8_t RIGHT[] = {3, 200, 2, 0};
uint8_t FORWARD[] = {1, 100, 90, 2};
// int bsdk = 0;

int ctl_bit = 1;

void output(uint8_t arr[]){
  for(int i = 0; i < 4; i++){
    output_M[i] = arr[i];
  }
  // if(state_current != state_next){
  //     ctl_bit = 0;
  //     bsdk = 0;
  // }
  // if(bsdk < 10 & ctl_bit == 0){
  //   // Serial.print(bsdk);
  //   output_M[1] = 0;
  //   bsdk = bsdk + 1;
  // }else{
  //   bsdk = 0;
  //   ctl_bit = 1;
  // }

  // state_current = state_next;
  return;
}

// void movingAverage(){
//   if(count < MOVNG_AVERAGE_SIZE){
// 		N_array[count] = target_GPS[0];
// 		E_array[count] = target_GPS[1];
// 		yaw_array[count] = current_heading;

// 		moving_average[0] += target_GPS[0];
// 		moving_average[1] += target_GPS[1];
// 		moving_average[2] += current_heading;
// 		count ++;
//     output(IDLE);
// 		return;
// 	}
		
// 	moving_average[0] += target_GPS[0] - N_array[head[0]];
// 	moving_average[1] += target_GPS[1] - E_array[head[0]];
// 	moving_average[2] += current_heading - yaw_array[head[2]];

// 	for(int i = 0; i < 3; i++){
// 		head[i] = (head[i] + 1)%MOVNG_AVERAGE_SIZE;
// 		tail[i] = (tail[i] + 1)%MOVNG_AVERAGE_SIZE;
// 	}

// 	N_array[tail[0]] = target_GPS[0];
// 	E_array[tail[1]] = target_GPS[1];
// 	yaw_array[tail[2]] = current_heading;

// 	target_GPS[0] = moving_average[0]/MOVNG_AVERAGE_SIZE;
// 	target_GPS[1] = moving_average[1]/MOVNG_AVERAGE_SIZE;
// 	current_heading = moving_average[2]/MOVNG_AVERAGE_SIZE;
//   return;
// }

double calculate_heading(double* target_GPS){
	if(target_GPS[1] >= 0){
		return 90 - atan(target_GPS[0]/target_GPS[1])*57.2957795131;
	}
	else{
		return -90 - atan(target_GPS[0]/target_GPS[1])*57.2957795131;
	}
}

void correct_heading(double current_heading, double target_heading){

	if (current_heading > target_heading){
    state_next = 1;
      output(LEFT);
      // Serial.println();
      // Serial.print("going LEFT");
    return;
	}
	else{
    state_next = 1;
      output(RIGHT);
      // Serial.println();
      // Serial.print("going RIGHT");
    return;
	}
}

void Manual(uint8_t mode, int throttle, int steering){


	int speed, modifier;
	uint8_t direction = 1;

	speed = map(throttle, 950, 2050, -MAX_SPEED , MAX_SPEED);

	if(abs(speed) < ERR_SPEED){speed = 0; direction = 1;}  //speed err
	else if(speed < 0){
		speed =abs(speed);
		direction = 0;
	}else{
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
			direction = 1;
			break;

		default :
			mode = 1;
			speed = 0;
			modifier = 90;
			direction = 1;
			break;
	}

	output_M[0] = mode;
	output_M[1] = speed;
	output_M[2] = modifier;
	output_M[3] = direction;

// output_M[0] = 3;
// 	output_M[1] = 0;
// 	output_M[2] = 2;
// 	output_M[3] = 0;
  // state_next = 1;
  // output(LEFT);

	return ;
}

// void Autonomous (double target_GPS[2],  double current_heading){

// 	// Implement queue for moving average
//   // movingAverage();

// 	//convert target hsetDirection(ch2Value,mode)eading to degrees
// 	double target_heading = calculate_heading(target_GPS);

// 	//Is reached
// 	if(-2 < target_GPS[0] && target_GPS[0] < 2 && -2 < target_GPS[1] && target_GPS[1] < 2){
//         state_next = 0;
//       	output(IDLE);
//       // Serial.println();
//       // Serial.print("going IDLE");
//     return;
// 	}
// 	//correct heading if its off
// 	if(!(target_heading - ERR_HEADING < current_heading && current_heading < target_heading + ERR_HEADING)){
// 		correct_heading(current_heading, target_heading);
//     return;
// 	}
//     state_next = 2;
//     output(FORWARD);
//       // Serial.println();
//       // Serial.print("going FORWARD");
// 	return;

// }




