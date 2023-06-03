#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  120 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  660 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 60 // Analog servos run at ~50 Hz updates

#define RC_steering_pin 2
int steering_slot_old = 0;

int FR = 0;
int FL = 1;
int BR = 4;
int BL = 5;

// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("8 channel Servo test!");
  pinMode(RC_steering_pin, INPUT);

  pwm.begin();
  pwm.setOscillatorFrequency(25000000);
  pwm.setPWMFreq(60);  // Analog servos run at ~50 Hz updates

  delay(10);
}



void loop() {

  int steering = pulseIn(RC_steering_pin, HIGH);
  Serial.println(steering);

  steering = map(steering , 1000 , 1980 , SERVOMIN, SERVOMAX);

  ackermann(steering);
  delay(10);

}


int assign_steering_slot(int steering){
  /*
    INPUT 
      steering - The input steering from the receiver

    OUTPUT (modifes global variable steering_slot)
      steering_slot - the steering slot in which the steering falls while using the receiver.

    Steering has 9 target radii: -60 -90 -150 -200 inf 200 150 90 60.
  */
  int steering_slot = 0;

  if (steering < 190){
    steering_slot = -4;
  }
  else if (steering >= 170 && steering < 250){
    steering_slot = -3; 
  }
  else if (steering >= 230 && steering < 310){
    steering_slot = -2; 
  }
  else if (steering >= 290 && steering < 370){
    steering_slot = -1; 
  }
  else if (steering >= 350 && steering < 430){
    steering_slot = 0; 
  }
  else if (steering >= 410 && steering < 490){
    steering_slot = 1; 
  }
  else if (steering >= 470 && steering < 550){
    steering_slot = 2; 
  }
  else if (steering >= 530 && steering < 610){
    steering_slot = 3; 
  }
  else if (steering >= 590){
    steering_slot = 4; 
  }

  return steering_slot;

}

int ackermann(int steering){
  /*
    INPUT
      steering_slot

    OUTPUT
      Sets the servo angles

    Implements Ackermann Steering based on the steering slot.
  */
  
  int steer = assign_steering_slot(steering);
  int FR_angle = 390;
  int FL_angle = 390;
  int BR_angle = 390;
  int BL_angle = 390;
  
  if(steering_slot_old != steer){
    if(steer == -4){
      FR_angle = 495;
      FL_angle = 453;
      BR_angle = 285;
      BL_angle = 326;
    }
    else if(steer == -3){
      FR_angle = 458;
      FL_angle = 436;
      BR_angle = 322;
      BL_angle = 343;
    }
    else if(steer == -2){
      FR_angle = 429;
      FL_angle = 420;
      BR_angle = 351;
      BL_angle = 359;
    }
    else if(steer == -1){
      FR_angle = 418;
      FL_angle = 413;
      BR_angle = 362;
      BL_angle = 366;
    }
    else if(steer == 1){
      FR_angle = 362;
      FL_angle = 366;
      BR_angle = 418;
      BL_angle = 413;
    }
    else if(steer == 2){
      FR_angle = 351;
      FL_angle = 359;
      BR_angle = 429;
      BL_angle = 420;
    }
    else if(steer == 3){
      FR_angle = 322;
      FL_angle = 343;
      BR_angle = 458;
      BL_angle = 436;
    }
    else if(steer == 4){
      FR_angle = 285;
      FL_angle = 326;
      BR_angle = 495;
      BL_angle = 453;
    }
    else{
      FR_angle = 390;
      FL_angle = 390;
      BR_angle = 390;
      BL_angle = 390;
    }
    pwm.setPWM(FL, 0, FL_angle);
    pwm.setPWM(FR, 0, FR_angle);
    pwm.setPWM(BL, 0, BL_angle);
    pwm.setPWM(BR, 0, BR_angle);
  }

  steering_slot_old = steer;
  
}
