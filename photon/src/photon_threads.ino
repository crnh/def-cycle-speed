#include "MQTT.h"
#include "mqtt_config.h"

const char datatopic[] = "bike/data";
const char statustopic[] = "bike/status";

const int timeout = 3000; // maximum measurement time in microseconds
const double dx = 1.0; // distance between both sensors in meters
const unsigned long sleepTimeout = 60; // time to stay awake after measurement in seconds.

MQTT client(MQTT_HOST, MQTT_PORT, callback);

Thread thread("mqttThread", MQTTSend);

// receive message
void callback(char *topic, byte *payload, unsigned int length)
{
}

//boolean hasFrontWheelPassedOne[] = {false, false, false, false};
//boolean hasFrontWheelPassedBoth[] = {false, false, false, false};
//boolean hasRearWheelPassedOne[] = {false, false, false, false};
int firstSensor[] = {0, 0, 0, 0};

unsigned long frontWheelTime[] = {0, 0, 0, 0};
unsigned long rearWheelTime[] = {0, 0, 0, 0};
unsigned long lastInterruptTime[] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long timestamp[4];

unsigned long lastMeasurementTime;

double frontWheelVelocity[] = {0.0, 0.0, 0.0, 0.0};

String dataToSend = "";

void setup()
{
    Serial.begin(115200);
    Serial.println("test");
  
    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);
    pinMode(D3, INPUT_PULLUP);
    pinMode(D4, INPUT_PULLUP);
    pinMode(D5, INPUT_PULLUP);
    pinMode(D6, INPUT_PULLUP);
    pinMode(D7, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);

    attachInterrupt(D1, velocityMeasure0A, FALLING);
    attachInterrupt(D2, velocityMeasure0B, FALLING);
    attachInterrupt(D3, velocityMeasure1A, FALLING);
    attachInterrupt(D4, velocityMeasure1B, FALLING);
    attachInterrupt(D5, velocityMeasure2A, FALLING);
    attachInterrupt(D6, velocityMeasure2B, FALLING);
    attachInterrupt(D7, velocityMeasure3A, FALLING);
    attachInterrupt(A2, velocityMeasure3B, FALLING);

    lastMeasurementTime = Time.now();
    SYSTEM_THREAD(ENABLED);
}

void loop()
{
    for (int j = 0; j < 4; j++)
    {
        if (millis() - frontWheelTime[j] > 3000 && frontWheelTime[j] != 0)
        { // een meting die langer duurt dan 3 seconden wordt afgebroken.
            resetSegment(j);
            Serial.println("expire \n");
        }
    }
    if (Time.now() - lastMeasurementTime > sleepTimeout){
        //Particle.sleep();
    }
    delay(10); 
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
            client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);
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

void velocityMeasure0A()
{
    debounceAndMeasure(0, 0);
}

void velocityMeasure0B()
{
    debounceAndMeasure(0, 1);
}

void velocityMeasure1A()
{
    debounceAndMeasure(1, 0);
}

void velocityMeasure1B()
{
    debounceAndMeasure(1, 1);
}

void velocityMeasure2A()
{
    debounceAndMeasure(2,0);
}

void velocityMeasure2B()
{
    debounceAndMeasure(2, 1);
}

void velocityMeasure3A()
{
    debounceAndMeasure(3, 0);
}

void velocityMeasure3B()
{
    debounceAndMeasure(3, 1);
}

void debounceAndMeasure(int segment, int sensor){
    Serial.println("Triggered");
    int index = 2*segment + sensor;
    if (millis() - lastInterruptTime[index] > 100)
    {
        Serial.println("Triggered " + String(segment) + "," + String(sensor));
        velocityMeasure(segment, sensor);
        lastInterruptTime[index] = millis();
    }
}

void velocityMeasure(int i, int sensor)
{
    if (frontWheelTime[i] == 0)
    { // first detection of bike, measurement starts
        timestamp[i] = Time.now();
        frontWheelTime[i] = millis();
        firstSensor[i] = sensor;
    }
    else if (sensor == firstSensor[i])
    { // rear wheel detected for the first time
        rearWheelTime[i] = millis();
    }
    else
    {
        int dt;
        if (frontWheelVelocity[i] == 0.0)
        { // second detection of front wheel, calculate front wheel speed
            //hasFrontWheelPassedBoth[i] = true;
            unsigned long frontWheelTimeTwo = millis();
            Serial.println(frontWheelTime[i]);
            Serial.println(frontWheelTimeTwo);
            dt = frontWheelTimeTwo - frontWheelTime[i];
            frontWheelVelocity[i] = 1000 * dx / dt;
            //Serial.println(frontWheelVelocity[i]);
        }

        else
        { // second detection of rear wheel, calculate rear wheel speed and send average speed
            if(rearWheelTime[i] != 0){
                unsigned long rearWheelTimeTwo = millis();
                dt = rearWheelTimeTwo - rearWheelTime[i];
                double rearWheelVelocity = 1000 * dx / dt;
                Serial.println(rearWheelTime[i]);
                Serial.println(rearWheelTimeTwo);
                sendVelocity(timestamp[i], (frontWheelVelocity[i] + rearWheelVelocity) / 2, i, firstSensor[i]); // send average of front and rear wheel speed
                Serial.println();
            }
            lastMeasurementTime = Time.now();
            resetSegment(i); // stop measurement
        }
        
    }
}

void sendVelocity(long timestamp, double velocity, int segment, int direction)
{
    //Serial.println(velocity);
    int velocitySign = direction*2 - 1; // determine the sign of the velocity
    dataToSend = String(timestamp) + "," + String(velocitySign * velocity) + "," + String(segment);
    /*
    if (direction == 0)
    {
        
    }
    else if (direction == 1)
    {
        dataToSend = "{\"vel\": -" + String(velocity) + ", \"seg\": " + String(segment) + ", \"time\": " + String(timeStamp) + " }";
    }*/
    Serial.println(dataToSend);
}

void resetSegment(int i)
{
    timestamp[i] = 0;

    firstSensor[i] = 0;

    frontWheelTime[i] = 0;
    rearWheelTime[i] = 0;

    frontWheelVelocity[i] = 0.0;
}

/*void startMQTT(char ip[]){
    ipAdress = ip;
    
}*/
