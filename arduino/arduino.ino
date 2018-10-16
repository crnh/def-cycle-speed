boolean hasFrontWheelPassedOne = false;
boolean hasFrontWheelPassedBoth = false;
boolean hasRearWheelPassedOne = false;
int firstSensor;


unsigned long frontWheelTime = 0;
unsigned long rearWheelTime = 0;

double frontWheelVelocity = 0.0;



int dx = 0.5; //afstand tussen beide sensoren in meter

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), interrupt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(3), interrupt, RISING);
  

}

void loop() {
  if(frontWheelTime - millis > 3000){ // een meting die langer duurt dan 3 seconden wordt afgebroken.
    reset(); 
  }
  delay(100);
}

void interrupt(){
  Serial.println("TRIGGERED");
}

void velocityMeasure0(){
  velocityMeasure(0);
}

void velocityMeasure1(){
  velocityMeasure(1);
}

void velocityMeasure(int sensor){
  if(!hasFrontWheelPassedOne){ // de fiets is voor de eerste keer gedetecteerd, de meting begint
    frontWheelTime = millis();
    firstSensor = sensor;
    hasFrontWheelPassedOne = true;
  }
  else if(sensor == firstSensor){ // het acchterwiel wordt voor de eerste keer gedetecteerd
    hasRearWheelPassedOne == true;
    rearWheelTime = millis();
  }
  else{
    int dt;
    if(hasFrontWheelPassedBoth){    //Berekening snelheid van het achterwiel.
      dt = millis() - rearWheelTime;
      double rearWheelVelocity = dx/dt;
      sendVelocity((frontWheelVelocity + rearWheelVelocity)/2, 1); // de gemiddelde snelheid van voor- en achterwiel wordt doorgestuurd
      reset(); // de meting stopt.
    }
    else{ //Berekening snelheid van het voorwiel. 
      hasFrontWheelPassedBoth = true;
      dt = millis() - frontWheelTime;
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







