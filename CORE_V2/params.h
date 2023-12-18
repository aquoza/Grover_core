//I2C params
#define SLAVE_ADDRESS = 0x08;


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
uint8_t FORWARD[] = {1, 100, 90, 0};

vector<double> target_GPS(2, 0);
vector<double> current_GPS(2, 0);
double current_heading;

int MAX_SPEED = 200; //0 - 255
int ERR_SPEED = 20; //0 - 255
int ERR_ACKERMANN = 6; // 0 - 255
int ERR_HEADING = 10; // in degrees
int MOVNG_AVERAGE_SIZE = 10;
