#include <SD.h>
#include <SPI.h> 

int misoPin = 50;
int mosiPin = 51;
int clkPin = 52;
int csPin = 40;

const int load_cell_pins[4] = {A0, A1, A2, A3};

const char* filename = "loadcell.csv";

File logFile; //creates SD card

void setup() {
  Serial.begin(115200);
  pinMode(csPin, OUTPUT);
  
  if(SD.begin(csPin)){
    Serial.println("card initiated");
  }
  else{
    Serial.println("failed");
  }

  logFile = SD.open(filename, FILE_WRITE); //opens SD card

  if (logFile) {
    logFile.println("timestamp_ms, V1, V2, V3, V4");
    logFile.flush(); //forces the data to be written immediately to the SD card rather than waiting and buffering
  } 
  else {
    Serial.println("failed to open file");
  }
}

void loop() {
  unsigned long timestamp = millis();

  float voltages[4];
  
  for(int i = 0; i < 4; i++){
    int rawVoltage = analogRead(load_cell_pins[i]); //goes through array of analog pin values and reads data from there
    voltages[i] = (rawVoltage * 5.0) / 1023.0; // converts analog to digital and writes into array
  }

  logFile.print(timestamp);
  Serial.print(timestamp);
  for(int i = 0; i < 4; i++){ //for loop writes all the values into SD card
    logFile.print(", ");
    Serial.print(", ");
    logFile.print(voltages[i], 4);
    Serial.print(voltages[i], 4);
  }

  logFile.println();
  Serial.println();

  logFile.flush();  
}
