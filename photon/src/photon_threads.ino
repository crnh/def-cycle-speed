void setup()
{
  Serial.begin(9600);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);

    attachInterrupt(D1, interrupt, RISING);
    //attachInterrupt(D2, interrupt, RISING);
}

void loop(){
 Serial.println("NOt Triggered");
 delay(120);
}

void interrupt()
{
  Serial.println("Triggered");
}
/*
#include "MQTT.h"

MQTT client("145.94.196.251", 1883, callback);

Thread thread("mqttThread", MQTTSend);

// recieve message
void callback(char *topic, byte *payload, unsigned int length)
{
}

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

String dataToSend = "";

double frontWheelVelocity = 0.0;

int i = 0;

double dx = 0.025; // distance between both sensors

void setup()
{
    Serial.begin(9600);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);

    //attachInterrupt(D1, interrupt, RISING);
    //attachInterrupt(D2, interrupt, RISING);

    client.connect("photon");
    client.subscribe("/test");
    client.publish("/test", "hoi");
}

void loop()
{

    if (millis() - frontWheelTime > 3500 && hasFrontWheelPassedOne)
    { // een meting die langer duurt dan 3 seconden wordt afgebroken.
        resetR();
        Serial.println("expire \n");
    }
    delay(100);
}

void MQTTSend()
{ // function for multithreading
    while (true)
    {
        if (client.isConnected())
        {
            client.loop();
        }
        else
        {
            client.connect("photon");
        }
        client.publish("/test", dataToSend);
    }
}

void interrupt()
{
    Serial.println("Triggered");
    if (millis() - lastInterruptTime0 > 50)
    {
        
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
            sendVelocity((frontWheelVelocity + rearWheelVelocity) / 2, 1, firstSensor); // send average of front and rear wheel speed
            Serial.println();
            resetR(); // stop measurement
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

void sendVelocity(double velocity, int segment, int direction)
{
    Serial.println(velocity);
    if(direction == 0){
        dataToSend = "{ \"velocity\": " + String(velocity) + ", \"segment\": " + String(segment) + "}";
    }
    else if(direction == 1){
        dataToSend = "{ \"velocity\": -" + String(velocity) + ", \"segment\": " + String(segment) + "}";
    }
}

void resetR()
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
*/