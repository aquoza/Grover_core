#include <Arduino.h>
//#include <cmath>
// Define the UART parameters
const int uartTxPin = 17;  // GPIO pin for UART TX
const int uartRxPin = 16;  // GPIO pin for UART RX
const int baudRate = 19200;

int arr[4] = {};

// Define Input Connections
#define CH1 3
#define CH2 5
#define CH3 6
#define CH4 9
#define CH5 10
#define CH6 11

// Integers to represent values from sticks and pots
int ch1Value;
int ch2Value;
int ch3Value;
int ch4Value;
int ch5Value;
uint8_t mode;
uint8_t speed;
uint8_t modifier;
uint8_t direction;



int maxLimit = 100;
int minLimit = -100;


// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
int readChannel(int channelInput, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the switch channel and return a boolean value
int setMode(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, intDefaultValue);
  if (ch < -25) { return 1; }
  else if(ch>-25 && ch<25){ return 2;}
  else{ return 3;}
}

int setSpeed(int channelOutput){
  return map(channelOutput,-100,100,0,100);
}

int setModifier(int channelOutput, int mode){
  if(mode==1){
    // Setting angle of wheel 
    if(abs(channelOutput)<3) return 90;
    else return map(channelOutput,-100,97,0,180);
  }
  else if(mode==2){
    if(abs(channelOutput)<3) return 127;
    else if(channelOutput<-3) return round(-1*20*cos((channelOutput*1.47)/100))+40;
    else return round(1*20*cos((channelOutput*1.47)/100))+40;
  }

  else if (mode==3) return 0;  // 0-left, 1-nothing, 2-right
}

int setDirection(int channelOutput, int mode){
  if(abs(channelOutput)<3) return 1;
  else return map(channelOutput,-100,100,0,3);
}

void setup() {
  // Set up serial monitor for showing stick values
  Serial.begin(baudRate);
  // Set all pins as inputs
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);
}



void loop() {

  // Get values for each channel
  ch1Value = readChannel(CH1, 0);
  ch2Value = readChannel(CH2, 0);
  ch3Value = readChannel(CH3, -100);
  ch4Value = readChannel(CH4, 0);
  ch5Value = readChannel(CH5, 0);
  mode = setMode(CH6,false);
  speed=setSpeed(ch3Value);
  modifier=setModifier(ch1Value,mode);
  direction=setDirection(ch2Value,mode);
  
  uint8_t arr[4]={mode,speed,modifier,direction};
  int arraySize = 4;

  // Send the array over UART
  Serial.write(arr, sizeof(int) * arraySize);
  // Serial.println(); // Add a newline character at the end of the transmission
  // Serial.print(modifier);
  // Serial.print(" | ");
  // Serial.print(direction);
  // Serial.print(" | ");
  // Serial.print(speed);
  // Serial.print(" | ");
  // Serial.print(ch4Value);
  // Serial.print(" | ");
  // Serial.print(ch5Value);
  // Serial.print(" | ");
  // Serial.println(mode);

  delay(100);  // Adjust the delay based on your requirements
}
