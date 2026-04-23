//Version 1 - Made for ready to drive circuit MK1 2024 it works dont fuck with this dipshit
int brake = 5; //Define brake switch input pin 5
int button = 4; //define ready to drive pin input pin 4
int pedal = 3; // Pedal position sensor output pin 3
int rtds = 2; // define ready to drive sound output pin 2
bool x = false; // Fuckoff button in code of course :)

void setup() {
  // put your setup code here, to run once:
    pinMode  (brake, INPUT); // configures brake switch as input
    digitalWrite(brake, LOW); // sets brake switch inital as low (0v on pin5)
    pinMode  (button, INPUT); // configures button switch as input
    digitalWrite(button, LOW); // sets button switch as low (0v on pin 4)
    pinMode  (pedal, OUTPUT); // configures pedal as output
    pinMode  (rtds, OUTPUT); // configures ready to drive sound as output
    delay(6000); // waits 6 seconds so car dont did Very important, if you die oh well womp womp
}

void loop() {
  if (x == false && digitalRead(brake) == HIGH && digitalRead(button) == HIGH)  // checks if brake and ready to drive botton are pressed/on
  {
    digitalWrite(rtds, HIGH); // turns on ready to drive horn
    digitalWrite(pedal, HIGH);// Turns on the pedal position sensor shitz
    delay(500); // waits 2 seconds
    digitalWrite(rtds, LOW); // turns off the horn cause its loud
    x = true;
  }
}
