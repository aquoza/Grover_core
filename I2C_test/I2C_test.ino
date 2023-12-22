#include <Wire.h>


byte dataGet[6]; //input data from RPI

void recieveData()
{
      Serial.println(); // Add a newline character at the end of the transmission
      Serial.print("in recieveData");
      int i = 0;
		while (Wire.available()){
			dataGet[i] = Wire.read(); 
      Serial.println();
      Serial.print(dataGet[i]);
      i++;
    }
}

void setup() {
  // put your setup code here, to run once:
	Wire.begin(0x08);
	Wire.onReceive(recieveData);

	Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.print("out recieveData");
  // Serial.println();
  // delay(100);

}
