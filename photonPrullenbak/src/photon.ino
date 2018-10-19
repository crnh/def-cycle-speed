/*
 * Project photon
 * Description:
 * Author:
 * Date:
 */

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin(9600);
  Serial.println("STARTED");
  pinMode(D1, INPUT);
  attachInterrupt(D1, sendTrigger, CHANGE);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  delay(1000);
  //Particle.publish("trigger", "NOT TRIGGERED");
  Serial.println("NOT TRIGGERED");
}

void sendTrigger() {
  //Particle.publish("trigger", "TRIGGERED");
  Serial.println("TRIGGERED");
}
