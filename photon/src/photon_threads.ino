#include "MQTT.h"

MQTT client("145.94.168.78", 1883, callback);

Thread thread("mqttThread", MQTTSend);

const char datatopic[] = "/bike";

// receive message
void callback(char *topic, byte *payload, unsigned int length)
{
}

boolean hasFrontWheelPassedOne[] = {false, false, false, false};
boolean hasFrontWheelPassedBoth[] = {false, false, false, false};
boolean hasRearWheelPassedOne[] = {false, false, false, false};
int firstSensor[] = {0, 0, 0, 0};

unsigned long frontWheelTime[] = {0, 0, 0, 0};
unsigned long rearWheelTime[] = {0, 0, 0, 0};
unsigned long lastInterruptTime0[] = {0, 0, 0, 0};
unsigned long lastInterruptTime1[] = {0, 0, 0, 0};

double frontWheelVelocity[] = {0.0, 0.0, 0.0, 0.0};

String dataToSend = "";

const double dx = 0.025; // distance between both sensors

void setup()
{
    Serial.begin(9600);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);

    attachInterrupt(D1, velocityMeasure0, FALLING);
    attachInterrupt(D2, velocityMeasure1, FALLING);

    client.connect("photon");
    client.subscribe("/test");
    client.publish("/test", "hoi");
}

void loop()
{
    for (int j = 0; j < 4; j++)
    {
        if (millis() - frontWheelTime[j] > 3000 && hasFrontWheelPassedOne[j])
        { // een meting die langer duurt dan 3 seconden wordt afgebroken.
            resetR(j);
            Serial.println("expire \n");
        }
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
        if(!dataToSend.equals("")){
            client.publish(datatopic, dataToSend);
            dataToSend = "";
        }
    }
}

/*void interrupt()
{
    Serial.println("Triggered");
    if (millis() - lastInterruptTime0 > 50)
    {
        
        lastInterruptTime0 = millis();
    }
}*/

void velocityMeasure0()
{

    if (millis() - lastInterruptTime0[0] > 50)
    {
        velocityMeasure(0, 0);
        lastInterruptTime0[0] = millis();
    }
}

void velocityMeasure1()
{
    if (millis() - lastInterruptTime1[0] > 50)
    {
        velocityMeasure(1, 0);
        lastInterruptTime0[0] = millis();
    }
}

void velocityMeasure(int sensor, int i)
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
            sendVelocity((frontWheelVelocity[i] + rearWheelVelocity) / 2, i, firstSensor[i]); // send average of front and rear wheel speed
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

void sendVelocity(double velocity, int segment, int direction)
{
    Serial.println(velocity);
    if (direction == 0)
    {
        dataToSend = "{ \"velocity\": " + String(velocity) + ", \"segment\": " + String(segment) + "}";
    }
    else if (direction == 1)
    {
        dataToSend = "{ \"velocity\": -" + String(velocity) + ", \"segment\": " + String(segment) + "}";
    }
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
