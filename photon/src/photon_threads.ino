#include "MQTT.h"

// CONSTANTS (CONFIGURATION)
double dx = 0.5;                   // distance between sensors in meter
int minTimeBetweenInterrupts = 60; // minimum time between two interrupts

MQTT client("145.94.196.251", 1883, callback);

//Thread thread("mqttThread", MQTTSend);

// recieve message
void callback(char *topic, byte *payload, unsigned int length)
{
}

// Define Segment class
#pragma region

class Segment
{
  public:
    int segmentId;
    long timestamp;
    long triggerTimes[4];

    Segment(int id);

    void getTriggerTimeA();
    void getTriggerTimeB();
    double calculateSpeed();
    void reset();

  private:
    long lastTriggerTimeA;
    long lastTriggerTimeB;
};

Segment::Segment(int id)
{
    segmentId = id;
}

void Segment::getTriggerTimeA()
{
    long triggerTime = millis();
    if (triggerTime - lastTriggerTimeA > minTimeBetweenInterrupts)
    {
        if (triggerTimes[0] == 0)
        {
            triggerTimes[0] = triggerTime;
        }
        else if (triggerTimes[1] == 0)
        {
            triggerTimes[1] = triggerTime;
        }
    }

    lastTriggerTimeA = triggerTime;
}

void Segment::getTriggerTimeB()
{
    long triggerTime = millis();
    if (triggerTime - lastTriggerTimeB > minTimeBetweenInterrupts)
    {
        if (triggerTimes[2] == 0)
        {
            triggerTimes[2] = triggerTime;
        }
        else if (triggerTimes[3] == 0)
        {
            triggerTimes[3] = triggerTime;
        }
    }

    lastTriggerTimeB = triggerTime;
}

double Segment::calculateSpeed()
{
    double speed1 = dx / (triggerTimes[2] - triggerTimes[0]);
    double speed2 = dx / (triggerTimes[3] - triggerTimes[1]);
    return (speed1 + speed2) / 2;
}

void Segment::reset()
{
    timestamp = 0;

    for (int i = 0; i < 4; i++)
    {
        triggerTimes[i] = 0;
    }
}

#pragma endregion Segment class definition

// Declare Segment class instances
#pragma region

Segment segment1 = Segment(1);

void getTriggerTimeSegment1A()
{
    segment1.getTriggerTimeA();
}

void getTriggerTimeSegment1B()
{
    segment1.getTriggerTimeB();
}

#pragma endregion Declare Segment Class Instances

void setup()
{
    // Segment 1
    pinMode(D1, INPUT_PULLUP);
    attachInterrupt(D1, getTriggerTimeSegment1A, RISING);

    pinMode(D2, INPUT_PULLUP);
    attachInterrupt(D2, getTriggerTimeSegment1B, RISING);

    //Segment 2

    //Segment 3

    //Segment 4
}

void loop() {

}