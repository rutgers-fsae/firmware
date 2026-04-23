//Apps Board 2026 v0.0.1 - Jeevan Shah
#define MAX_IMPLAUSIBILITY_DURATION 100 // duration (in msec) that an implausibility is allowed to last before giving a fault - need to look into this
int lp1 = A0; //sets linear pot 1 as linpot1(analog)
int lp2 = A1; //sets linear pot 2 as linpot2 (analog)
int bs = 2;  //sets brakesw as Brake switch input (Digital)
int out = 3;   //sets output as the output womp womp

/* set_implausibility: Set implausibility hardware pin 
 * No input parameters
 * No return value
 */
void set_implausibility(bool plaus) {
  // is there is implausibility the output should be LOW
  if(plaus == true){
    digitalWrite(out, LOW);
  } else {
    digitalWrite(out, HIGH);
  }
}

/* read_brake_switch: Reads the brake switch pin 
 * No input paramters
 * Returns an integer, HIGH or LOW
 */
bool read_brake_switch() {
  return digitalRead(bs);
}

/*
 * sensor_transfer_fcn: Converts raw ADC value to accelerator position percentage
 * rawVal (unsigned 16-bit number): Raw value from ADC output
 * sensorNum (unsigned 8-bit number): Physical sensor number 1 or 2 on the board
 * outputPercentage (float): Number from 0-100% representing the pedal position
 */
float sensor_transfer_fcn(int rawVal, int sensorNum) {
  float outputPercentage, voltage;
  voltage = 5.0 * rawVal / 1023.0;
  switch (sensorNum) {
    case 1:
      return (40 * voltage) - 50;
      break;
    case 2:
      return (40 * voltage) - 33;
      break;
    default:
      Serial.println("Invalid sensor number");
      return -1;
      break;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(lp1, INPUT); //Lin pot 1 as an input
  digitalWrite(lp1, LOW); //Lp1 as low

  pinMode(lp2, INPUT); //Lin pot 2 as an input
  digitalWrite(lp2, LOW); //Lp1 as low

  pinMode(bs, INPUT); //bs as an input
  pinMode(out, OUTPUT); //out as an output

  digitalWrite(out, LOW); // default the system to have implausibility

  delay(100); // delay the start circuit slightly

int sensor1RawVal,sensor2RawVal;
float sensor1Percentage, sensor2Percentage;
bool implausibility = false, implausibilityLatched = false, prevMismatch = false, brakeSwitch;
unsigned long implausibilityStartTimer = 0, presentTimer = 0;

void loop() {
  presentTimer = millis(); // get current time in milliseconds

  // Read APPS sensor values
  sensor1RawVal = analogRead(lp1);
  sensor2RawVal = analogRead(lp2);
  
  // Serial.print("APPS 1 (Raw): ");
  // Serial.println(sensor1RawVal);

  // Seria.print("APPS 2 (Raw):");
  //Serial.println(sensor2RawVal);


  // Convert raw APPS values to percentages from 0-100
  sensor1Percentage = sensor_transfer_fcn(sensor1RawVal, 1);
  sensor2Percentage = sensor_transfer_fcn(sensor2RawVal, 2);

  bool appsRangeFault = (sensor1Percentage < 0 || sensor2Percentage < 0 || sensor1Percentage > 100 || sensor2Percentage > 100);
  if (appsRangeFault) {
    Serial.println("Invalid sensor percentages");
  }


  Serial.print("APPS 1: ");
  Serial.println(sensor1Percentage);

  Serial.print("APPS 2:");
  Serial.println(sensor2Percentage);

  // Check APPS mismatch fault timing
  bool mismatchNow = (fabsf(sensor1Percentage - sensor2Percentage) > 10);
  if (mismatchNow && !prevMismatch) {
    implausibilityStartTimer = presentTimer;
  }

  bool appsMismatchFault = mismatchNow && ((presentTimer - implausibilityStartTimer) >= MAX_IMPLAUSIBILITY_DURATION);
  prevMismatch = mismatchNow;

  brakeSwitch = read_brake_switch();

  // Printing brake switch state 
  // Serial.println(brakeSwitch);


  // if the brakeSwitch is HIGH there is a fault
  bool brakeThrottleFault = ((brakeSwitch == HIGH) && ((sensor1Percentage > 25) || (sensor2Percentage > 25)));

  bool activeFault = (appsRangeFault || appsMismatchFault || brakeThrottleFault);
  if (activeFault) {
    implausibilityLatched = true;
  }

  bool pedalReleased = ((sensor1Percentage < 10) && (sensor2Percentage < 10));
  if (implausibilityLatched && pedalReleased && !activeFault) {
    implausibilityLatched = false;
  }

  implausibility = implausibilityLatched;
  set_implausibility(implausibility);
}
