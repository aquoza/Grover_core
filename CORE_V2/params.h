#include <Arduino.h>
//I2C params
#define SLAVE_ADDRESS = 0x08;

// Define the UART parameters
const int uartTxPin = 17;  // GPIO pin for UART TX
const int uartRxPin = 16;  // GPIO pin for UART RX
const int baudRate = 19200;

// Define Input Connections
#define CH1 3
#define CH2 5 // throttle
#define CH3 6 
#define CH4 9 //direction
#define CH5 10 //autonomous
#define CH6 11 // mode



int MAX_SPEED = 100; //0 - 255
int ERR_SPEED = 20; //0 - 255
int ERR_ACKERMANN = 6; // 0 - 255
int ERR_HEADING = 5; // in degrees
int MOVNG_AVERAGE_SIZE = 10;
