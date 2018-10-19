
boolean hasFrontWheelPassedOne[] = {false, false, false, false};
boolean hasFrontWheelPassedBoth[] = {false, false, false, false};
boolean hasRearWheelPassedOne[] = {false, false, false, false};
int firstSensor[] = {0, 0, 0, 0};

unsigned long frontWheelTime[] = {0, 0, 0, 0};
unsigned long rearWheelTime[] = {0, 0, 0, 0};
unsigned long lastInterruptTime0[] = {0, 0, 0, 0};
unsigned long lastInterruptTime1[] = {0, 0, 0, 0};

double frontWheelVelocity[] = {0.0, 0.0, 0.0, 0.0};

int i = 0;

double dx = 0.025; // distance between both sensors

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT_PULLup);
  pinMode(3, INPUT_PULLup);

  attachInterrupt(D1, velocityMeasure0seg0, FALLING);
  attachInterrupt(D1, velocityMeasure1seg0, FALLING);
}

void loop()
{
  for(int i = 0; i < 4; i++){
    if(millis() - frontWheelTime[i] > 3000 && hasFrontWheelPassedOne[i] ){ // een meting die langer duurt dan 3 seconden wordt afgebroken.
      if(frontWheelVelocity != 0.00000){
        sendVelocity(frontWheelVelocity, i, false);
        /*
        The measurement can have a frontWheelVelocity when it expires, when:
          1) the bike has only one wheel. In this case, the measurement is correct but has a larger uncertainty.
          2) the front wheel was not detected at the first sensor and the bike is shorter than dx. The value of firstSensor is correct, but the normal speed calculation
              does not make any sense. The speed is is dx / (rearWheelTimeTwo - frontWheelTime). 
          3) the rear wheel was not detected at the second sensor. In this case, the measurement is correct but has a larger uncertainty.
          4) the front wheel was not detected at the second sensor. In this case, the normal speed calculation does not make any sense. In this case 
              the speed is dx / (frontWheelTimeTwo - rearWheelTime - )

        The measurement don't have a frontWheelVelocity when:
          1) a sensor was triggered by a non-bike thing or an error.
          2) a sensor does detect the front wheel nor the rear wheel;
        */
      }
      resetR(i);
      Serial.println("expire \n");
    }
  }
  delay(100);
}

void velocityMeasure0seg0()
{
  debounceAndMeasure(0, 0);
  
}

void velocityMeasure1seg0()
{
  debounceAndMeasure(1, 0); 
}

void debounceAndMeasure(int sensor, int i){
  boolean con = false;
  if(sensor == 0){
    if( millis() - lastInterruptTime0[i] > 50){
      velocityMeasure(sensor, i);
      lastInterruptTime0[i] = millis();
    }
  }
  else if (sensor == 1){
    if(millis() - lastInterruptTime1[i] > 50){
      velocityMeasure(sensor, i);
      lastInterruptTime1[i] = millis();
    }
  }
}

void velocityMeasure(int sensor, int i)  // where i is the segment, an int between 0 and 3 
{
  if (!hasFrontWheelPassedOne[i])
  { // first detection of bike, measurement starts
    frontWheelTime[i] = millis();
    firstSensor[i] = sensor;
    hasFrontWheelPassedOne[i] = true;
  }
  else if (sensor == firstSensor[i])
  { // rear wheel detected for the first time
    hasRearWheelPassedOne[i] = true;
    rearWheelTime[i] = millis();
  }
  else
  {
    int dt;
    if (hasFrontWheelPassedBoth[i])
    { // calculate rear wheel speed
      unsigned long rearWheelTimeTwo = millis();
      dt = rearWheelTimeTwo - rearWheelTime[i];
      double rearWheelVelocity = 1000 * dx / dt;
      Serial.println(rearWheelTime[i]);
      Serial.println(rearWheelTimeTwo);
      if(rearWheelTime == 0){
        sendVelocity(frontWheelVelocity[i], i, false); // send average of front and rear wheel speed
        /*
        If the rearWheelTime equals zero, there was no second detection on the first sensor. This happens when:
        
          1) the rear wheel of the bike is not detected at the first sensor. In this case, the measurement is correct but has a larger uncertainty.
          2) the front wheel was not detected at the first sensor and the bike is longer than dx. The value of firstSensor is wrong, so the normal speed calculation
              does not make any sense. The speed is is dx / (frontWheelTimeTwo - rearWheelTime). 
        */
      }
      else{
        sendVelocity((frontWheelVelocity[i] + rearWheelVelocity) / 2, i, true); // send average of front and rear wheel speed
      }
      Serial.println();
      resetR(i); // stop measurement
    }

    else
    { // calculate front wheel speed
      hasFrontWheelPassedBoth[i] = true;
      unsigned long frontWheelTimeTwo = millis();
      Serial.println(frontWheelTime[i]);
      Serial.println(frontWheelTimeTwo);
      dt = frontWheelTimeTwo - frontWheelTime[i];
      frontWheelVelocity[i] = 1000 * dx / dt;
      //Serial.println(frontWheelVelocity);
    }
  }
}

void sendVelocity(double velocity, int segment, boolean isMean)
{
  Serial.println(velocity);
}

void resetR(int i)
{
  hasFrontWheelPassedOne[i] = false;
  hasFrontWheelPassedBoth[i] = false;
  hasRearWheelPassedOne[i] = false;
  firstSensor[i] = 0;

  frontWheelTime[i] = 0;
  rearWheelTime[i] = 0;

  frontWheelVelocity[i] = 0.0;
}
