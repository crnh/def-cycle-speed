void setup()
{
  Serial.begin(9600);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);

    attachInterrupt(D1, interrupt, RISING);
    //attachInterrupt(D2, interrupt, RISING);
}

void loop(){

}

void interrupt()
{
  Serial.println("Triggered");
}
