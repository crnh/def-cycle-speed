SYSTEM_THREAD(ENABLED);

const char datatopic[] = "bike/data";
const char statustopic[] = "bike/status";

const int timeout = 1000;              // maximum measurement time in microseconds
const double dx = 1.0;                 // distance between both sensors in meters
const unsigned long sleepTimeout = 100; // time to stay awake after measurement in seconds.

Thread thread("publishThread", publishToCloud);

// receive message


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

String dataToSendArray[128];

void setup()
{
    Serial.begin(115200);
    Serial.println("test");
    
    //RGB.control(true);
    //RGB.color(0, 0, 0);

    pinMode(D1, INPUT_PULLUP);
    pinMode(D2, INPUT_PULLUP);
    pinMode(D3, INPUT_PULLUP);
    pinMode(D4, INPUT_PULLUP);
    pinMode(D5, INPUT_PULLUP);
    pinMode(D6, INPUT_PULLUP);
    pinMode(D7, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(RX, INPUT_PULLUP);

    attachInterrupt(D1, velocityMeasure0A, FALLING);
    attachInterrupt(D2, velocityMeasure0B, FALLING);
    attachInterrupt(D3, velocityMeasure1A, FALLING);
    attachInterrupt(D4, velocityMeasure1B, FALLING);
    attachInterrupt(D5, velocityMeasure2A, FALLING);
    attachInterrupt(D6, velocityMeasure2B, FALLING);
    attachInterrupt(D7, velocityMeasure3A, FALLING);
    attachInterrupt(A2, velocityMeasure3B, FALLING);

    for (int i = 0; i < 128; i++)
    {
        dataToSendArray[i] = "";
    }

    lastMeasurementTime = Time.now();
}

void loop()
{
    for (int j = 0; j < 4; j++)
    {
        if (millis() - frontWheelTime[j] > 1300 && frontWheelTime[j] != 0)
        { // een meting die langer duurt dan 3 seconden wordt afgebroken.
            resetSegment(j);
            Serial.println("expire \n");
        }
    }
    if (Time.now() - lastMeasurementTime > sleepTimeout && dataToSendArray[0].equals("") && WiFi.ready())
    {
        Serial.println("sleep");
        WiFi.off();
    }
    //Serial.println(".");
    delay(10);
}

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
    debounceAndMeasure(2, 0);
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

void debounceAndMeasure(int segment, int sensor)
{
    Serial.println("triggered");
    int index = 2 * segment + sensor;
    if (millis() - lastInterruptTime[index] > 75)
    {
        Serial.println("triggered " + String(segment) + "," + String(sensor));
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
            Serial.println(frontWheelVelocity[i]);
        }

        else
        { // second detection of rear wheel, calculate rear wheel speed and send average speed
            if (rearWheelTime[i] != 0)
            {
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

void sendVelocity(unsigned long timestamp, double velocity, int segment, int direction)
{
    //Serial.println(velocity);
    int velocitySign = direction * 2 - 1; // determine the sign of the velocity
    dataToSend = String(timestamp) + "," + String(velocitySign * velocity) + "," + String(segment);

    for (int i = 0; i < 128; i++)
    {
        if (dataToSendArray[i].equals(""))
        {
            dataToSendArray[i] = dataToSend;
            break;
        }
    }

    /*
    if (direction == 0){}
    else if (direction == 1){
        dataToSend = "{\"vel\": -" + String(velocity) + ", \"seg\": " + String(segment) + ", \"time\": " + String(timeStamp) + " }";
    }*/
    Serial.println(dataToSend);
}

void publishToCloud()
{
    int length = 128;
    String stringToSend;
    int amountToSend;
    while (true)
    {
        if(WiFi.ready() || !dataToSendArray[4].equals("")){ // De connectie wordt pas hersteld als er 5 fietsen gemeten zijn.
            stringToSend = "";
            amountToSend = 0;
            for (int i = 0; i < length; i++)
            { // Up till 9 strings from dataToSendArray are combined in stringToSend, seperated by a comma
                if (dataToSendArray[i].equals("") || amountToSend >= 9)
                {
                    break;
                }
                stringToSend += dataToSendArray[i];
                amountToSend++;
                if (dataToSendArray[i + 1].equals("") || amountToSend >= 9)
                {
                    break;
                }
                else
                {
                    stringToSend += ";";
                }
            }
            for (int i = 0; i < length - amountToSend; i++)
            { // dataToSendArray elements are shifed to the left
                dataToSendArray[i] = dataToSendArray[i + amountToSend];
            }
            for (int i = length - amountToSend; i < length; i++)
            { // last places will be cleared
                dataToSendArray[i] = "";
            }
            if (amountToSend > 0)
            {   
                if(!WiFi.ready()){
                    WiFi.on();
                    while(!WiFi.ready()){
                        unsigned long connectTime = millis();
                        while(millis() - connectTime < 120000){
                            WiFi.connect();
                            delay(200);
                        }
                        delay(600000);
                    }
                }
                Particle.publish(datatopic, stringToSend);
            }
        }
        delay(1200);
    }
}

void resetSegment(int i)
{
    timestamp[i] = 0;

    firstSensor[i] = 0;

    frontWheelTime[i] = 0;
    rearWheelTime[i] = 0;

    frontWheelVelocity[i] = 0.0;
}

