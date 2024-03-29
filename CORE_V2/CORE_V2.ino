/** Arduino code for GRover_V2 **/

#include <Arduino.h>
#include <Wire.h>
#include "params.h"
#include "functions.h"

/**
  0 - targetgps_NHIGH 
  1 - targetgps_NLOW
  2 - targetgps_EHIGH
  3 - targetgps_ELOW
  4 - currentheadingHIGH
  5 - currentheadingLOW
 **/


void recieveData()
{
    int i = 0;
    int temp = 0;
	while(Wire.available()){
    if(temp == 0){
      temp++;
      auto a = Wire.read();
      continue;
    }
		dataGet[i] = Wire.read(); 
    i++;

	}
      // Serial.print(target_GPS[0]);
      // Serial.print(" | ");
      // Serial.print(target_GPS[1]);
      // Serial.print(" | ");
      // Serial.print(current_heading);
      // Serial.print(" | ");
}


void setup() {

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
	int throttle = pulseIn(CH2, HIGH, 30000);
	int steering = pulseIn(CH4, HIGH, 30000);
	int SwitchB = pulseIn(CH5, HIGH, 30000);
	int SwitchC = pulseIn(CH6, HIGH, 30000);

	int mode = 0;
	//Getting mode
	if(SwitchC < 1200){ mode = 1; }
	else if (1200 <= SwitchC && SwitchC < 1800){ mode = 2; }
	else if (SwitchC >= 1800){ mode = 3; }
	else { mode = 0; }

	//Autonomous or Manual depending on SwitchC
	int arraySize = 4;
	if(SwitchB > 1500){
    delay(100);
    Autonomous(target_GPS, current_heading);
		Serial.write(output_A, sizeof(int) * arraySize);
  //     // Serial.println();
  //     // Serial.print(output_A[0]);
  //     // Serial.print(" | ");
  //     // Serial.print(output_A[1]);
  //     // Serial.print(" | ");
  //     // Serial.print(output_A[2]);
  //     // Serial.print(" | ");
  //     // Serial.print(output_A[3]);
  //     // Serial.print(" | ");
	}
	else {
      Manual(mode, throttle, steering);
      Serial.write(output_M, sizeof(int) * arraySize);
      // Serial.println();
      // Serial.print(output_M[0]);
      // Serial.print(" | ");
      // Serial.print(output_M[1]);
      // Serial.print(" | ");
      // Serial.print(output_M[2]);
      // Serial.print(" | ");
      // Serial.print(output_M[3]);
      // Serial.print(" | ");
      // Serial.print(SwitchC);
		  delay(100);

	}

}
