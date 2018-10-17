boolean hasFrontWheelPassedOne = false;
boolean hasFrontWheelPassedBoth = false;
boolean hasRearWheelPassedOne = false;
int firstSensor;


unsigned long frontWheelTime = 0;
unsigned long rearWheelTime = 0;
unsigned long lastInterruptTime = 0;

double frontWheelVelocity = 0.0;

int i = 0;

int dx = 0.5; //afstand tussen beide sensoren in meter

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), interrupt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(2), setDown, FALLING);
  
}

void loop() {
  /*if(frontWheelTime - millis() > 3000){ // een meting die langer duurt dan 3 seconden wordt afgebroken.
    reset(); 
  }*/
  Serial.println("");
  delay(150);
}

void interrupt(){
  if(lastInterruptTime + 65 < millis()){
    Serial.println("Triggered");
    lastInterruptTime = millis();
  }
  
}

void velocityMeasure0(){
  if(lastInterruptTime0 + 65 < millis()){
    velocityMeasure(0);
    lastInterruptTime0 = millis();
  }
}

void velocityMeasure1(){
  if(lastInterruptTime0 + 65 < millis()){
    velocityMeasure(1);
    lastInterruptTime0 = millis();
  }
}

void velocityMeasure(int sensor){
  if(!hasFrontWheelPassedOne){ // de fiets is voor de eerste keer gedetecteerd, de meting begint
    frontWheelTime = millis();
    firstSensor = sensor;
    hasFrontWheelPassedOne = true;
  }
  else if(sensor == firstSensor){ // het achterwiel wordt voor de eerste keer gedetecteerd
    hasRearWheelPassedOne == true;
    rearWheelTime = millis();
  }
  else{
    int dt;
    if(hasFrontWheelPassedBoth){    //Berekening snelheid van het achterwiel.
      unsigned long rearWheelTimeTwo = millis();
      dt = rearWheelTimeTwo - rearWheelTime;
      double rearWheelVelocity = dx/dt;
      sendVelocity((frontWheelVelocity + rearWheelVelocity)/2, 1); // de gemiddelde snelheid van voor- en achterwiel wordt doorgestuurd
      reset(); // de meting stopt.
    }
    else{ //Berekening snelheid van het voorwiel. 
      hasFrontWheelPassedBoth = true;
      unsigned long frontWheelTimeTwo = millis();
      dt = frontWheelTimeTwo ;
      frontWheelVelocity = dx/dt;
    }
  }

}

void sendVelocity(double velocity, int segment){
  
}

void reset(){
  hasFrontWheelPassedOne = false;
  hasFrontWheelPassedBoth = false;
  hasRearWheelPassedOne = false;
  firstSensor;

  frontWheelTime = 0;
  rearWheelTime = 0;

  frontWheelVelocity = 0.0;
}








