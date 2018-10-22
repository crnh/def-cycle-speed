#include "MQTT.h"

SYSTEM_THREAD(ENABLED);

// CONSTANTS (CONFIGURATION)
double dx = 0.5;                   // distance between sensors in meter
int minTimeBetweenInterrupts = 60; // minimum time between two interrupts
int expireTime = 2000;             // time after which an unfinished measurement expires

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
    int id;

    // timestamp, indicating the time of the first trigger
    long timestamp;

    /* array containing the trigger times. 
        0: first trigger of pinA, 
        1: second trigger of pinA,
        2: first trigger of pinB,
        3: second trigger of pinB
    */
    long triggerTimes[4];

    Segment(int id);

    void getTriggerTimeA();
    void getTriggerTimeB();
    boolean measurementFinished();
    double getVelocity();
    void reset();

  private:
    long lastTriggerTimeA;
    long lastTriggerTimeB;
};

Segment::Segment(int segmentId)
{
    id = segmentId;
    for (int i = 0; i < 4; i++)
    {
        triggerTimes[i] = 0;
    }
}

// Push trigger times of pinA in milliseconds to triggerTimes
void Segment::getTriggerTimeA()
{
    Serial.println("TRIGGERED");
    long triggerTime = millis();
    if (triggerTime - lastTriggerTimeA > minTimeBetweenInterrupts)
    {
        Serial.println("TRIGGERED OK");
        if (triggerTimes[0] == 0)
        {
            Serial.println("TRIGGERED OK: first");
            triggerTimes[0] = triggerTime;
            if (triggerTimes[2] == 0)
            {
                timestamp = Time.now();
            }
        }
        else if (triggerTimes[1] == 0)
        {
            Serial.println("TRIGGERED OK: second");
            triggerTimes[1] = triggerTime;
        }
    }

    lastTriggerTimeA = triggerTime;
}

// Push trigger times of pinB in milliseconds to triggerTimes
void Segment::getTriggerTimeB()
{
    Serial.println("TRIGGERED");
    long triggerTime = millis();
    if (triggerTime - lastTriggerTimeB > minTimeBetweenInterrupts)
    {
        Serial.println("TRIGGERED OK");
        if (triggerTimes[2] == 0)
        {
            Serial.println("TRIGGERED OK: first");
            triggerTimes[2] = triggerTime;
            if (triggerTimes[0] == 0)
            {
                timestamp = Time.now();
            }
        }
        else if (triggerTimes[3] == 0)
        {
            Serial.println("TRIGGERED OK: second");
            triggerTimes[3] = triggerTime;
        }
    }

    lastTriggerTimeB = triggerTime;
}

// returns a bool indicating whether the measurement has been finished or not
boolean Segment::measurementFinished()
{
    for (long i : triggerTimes)
    {
        if (i == 0)
        {
            return false;
        }
    }
    Serial.println("measurement finished");
    return true;
}

// calculate bike speed from values in triggerTimes
double Segment::getVelocity()
{
    double speed1 = dx / (triggerTimes[2] - triggerTimes[0]);
    double speed2 = dx / (triggerTimes[3] - triggerTimes[1]);
    return (speed1 + speed2) / 2;
}

// clean all measurement data (timestamp and triggertimes)
void Segment::reset()
{
    Serial.println("RESET");
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
Segment segment2 = Segment(2);
Segment segment3 = Segment(3);
Segment segment4 = Segment(4);

void getTriggerTimeSegment1A()
{
    segment1.getTriggerTimeA();
}

void getTriggerTimeSegment1B()
{
    segment1.getTriggerTimeB();
}

void getTriggerTimeSegment2A()
{
    segment2.getTriggerTimeA();
}

void getTriggerTimeSegment2B()
{
    segment2.getTriggerTimeB();
}

void getTriggerTimeSegment3A()
{
    segment3.getTriggerTimeA();
}

void getTriggerTimeSegment3B()
{
    segment3.getTriggerTimeB();
}

void getTriggerTimeSegment4A()
{
    segment4.getTriggerTimeA();
}

void getTriggerTimeSegment4B()
{
    segment4.getTriggerTimeB();
}

#pragma endregion Declare Segment Class Instances

Segment segmentArray[] = {segment1};

void setup()
{
    Serial.begin(115200);

    // Segment 1
    pinMode(D1, INPUT_PULLUP);
    attachInterrupt(D1, getTriggerTimeSegment1A, RISING);

    pinMode(D2, INPUT_PULLUP);
    attachInterrupt(D2, getTriggerTimeSegment1B, RISING);

    //Segment 2

    //Segment 3

    //Segment 4
}

void loop()
{
    delay(100);

    if (segment1.measurementFinished())
    {
        Serial.println("measurement finished");
        sendValue(segment.id, segment.getVelocity());
        segment.reset();
    }

    // check if there are any expired measurements

    for (int i = 0; i < 4; i++)
    {
        triggerTime = segment1.triggerTimes[i];
        if (triggerTime != 0 && millis() - triggerTime > expireTime)
        {
            Serial.println("EXPIRED");
            segment.reset();
        }
    }
}

void sendValue(int id, double velocity)
{
    Serial.println("ID: " + String(id));
    Serial.println("velocity: " + String(velocity));
}