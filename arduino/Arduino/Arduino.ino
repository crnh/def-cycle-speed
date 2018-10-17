boolean hasFrontWheelPassedOne = false;
boolean hasFrontWheelPassedBoth = false;
boolean hasRearWheelPassedOne = false;
int firstSensor;

unsigned long frontWheelTime = 0;
unsigned long rearWheelTime = 0;
unsigned long lastInterruptTime0 = 0;
unsigned long lastInterruptTime1 = 0;

boolean isDoubleInterrupt0 = false;
boolean isDoubleInterrupt1 = false;


double frontWheelVelocity = 0.0;

int i = 0;

double dx = 0.025; // distance between both sensors

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), velocityMeasure0, RISING);
  attachInterrupt(digitalPinToInterrupt(3), velocityMeasure1, RISING);
}

void loop()
{
  if(millis() - frontWheelTime > 3500 && hasFrontWheelPassedOne){ // een meting die langer duurt dan 3 seconden wordt afgebroken.
    reset();
    Serial.println("expire \n");
  }
  delay(100);
}

void interrupt()
{
  if (millis() - lastInterruptTime0 > 50)
  {
    Serial.println("Triggered");
    lastInterruptTime0 = millis();
  }
}

void velocityMeasure0()
{
  
  if (millis() - lastInterruptTime0 > 50)
  {
    velocityMeasure(0);
    lastInterruptTime0 = millis();
  }
  
}

void velocityMeasure1()
{
  if (millis() - lastInterruptTime1 > 50)
  {
    velocityMeasure(1);
    lastInterruptTime0 = millis();
  }
  
}

void velocityMeasure(int sensor)
{
  if (!hasFrontWheelPassedOne)
  { // first detection of bike, measurement starts
    frontWheelTime = millis();
    firstSensor = sensor;
    hasFrontWheelPassedOne = true;
  }
  else if (sensor == firstSensor)
  { // rear wheel detected for the first time
    hasRearWheelPassedOne = true;
    rearWheelTime = millis();
  }
  else
  {
    int dt;
    if (hasFrontWheelPassedBoth)
    { // calculate rear wheel speed
      unsigned long rearWheelTimeTwo = millis();
      dt = rearWheelTimeTwo - rearWheelTime;
      double rearWheelVelocity = 1000 * dx / dt;
      Serial.println(rearWheelTime);
      Serial.println(rearWheelTimeTwo);
      sendVelocity((frontWheelVelocity + rearWheelVelocity) / 2, 1); // send average of front and rear wheel speed
      Serial.println();
      reset(); // stop measurement
    }

    else
    { // calculate front wheel speed
      hasFrontWheelPassedBoth = true;
      unsigned long frontWheelTimeTwo = millis();
      Serial.println(frontWheelTime);
      Serial.println(frontWheelTimeTwo);
      dt = frontWheelTimeTwo - frontWheelTime;
      frontWheelVelocity = 1000 * dx / dt;
      //Serial.println(frontWheelVelocity);
    }
  }
}

void sendVelocity(double velocity, int segment)
{
  Serial.println(velocity);
}

void reset()
{
  hasFrontWheelPassedOne = false;
  hasFrontWheelPassedBoth = false;
  hasRearWheelPassedOne = false;
  isDoubleInterrupt0 = false;
  isDoubleInterrupt1 = false;
  firstSensor = 0;

  frontWheelTime = 0;
  rearWheelTime = 0;

  frontWheelVelocity = 0.0;
}
