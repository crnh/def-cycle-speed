SYSTEM_THREAD(ENABLED);

const char datatopic[] = "bike/data";           // topic for publishing datasets
const char statustopic[] = "bike/status";
const int dataBufferSize = 128;                 // size of data buffer array used to queue messages

const int maximumMeasurementDuration = 1000;    // maximum measurement time in microseconds
const long connectTime = 180000;                // time allowed to get connection before timing out
const int minimumTimeBetweenPublishes = 1100;   // minimum time between two publishes to the Particle Cloud
const long noConnectionSleepTime = 300000;      // wait 5 minutes before re-connecting if no connection.
const double dx = 1.0;                          // distance between both sensors in meters
const unsigned long sleepTimeout = 100;         // time to stay awake after measurement in seconds.

// Start data publish thread
Thread thread("publishThread", publishToCloud);

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
    
    // Set pinmodes for interrupts
    pinMode(D2, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(D1, INPUT_PULLUP);
    pinMode(D4, INPUT_PULLUP);
    pinMode(D5, INPUT_PULLUP);
    pinMode(D6, INPUT_PULLUP);
    pinMode(B0, INPUT_PULLUP);
    pinMode(D3, INPUT_PULLUP);

    // Define interrupt pins
    attachInterrupt(D2, velocityMeasure0A, FALLING);
    attachInterrupt(A2, velocityMeasure0B, FALLING);
    attachInterrupt(D1, velocityMeasure1A, FALLING);
    attachInterrupt(D4, velocityMeasure1B, FALLING);
    attachInterrupt(D5, velocityMeasure2A, FALLING);
    attachInterrupt(D6, velocityMeasure2B, FALLING);
    attachInterrupt(B0, velocityMeasure3A, FALLING);
    attachInterrupt(D3, velocityMeasure3B, FALLING);

    // Initialize data buffer (Particle Cloud allows only 1 message per second)
    for (int i = 0; i < dataBufferSize; i++)
    {
        dataToSendArray[i] = "";
    }

    lastMeasurementTime = Time.now();
}

void loop()
{
    for (int j = 0; j < 4; j++)
    {
        // Reset if a measurement takes longer than the maximum measurement duration
        if (millis() - frontWheelTime[j] > maximumMeasurementDuration && frontWheelTime[j] != 0)
        {
            resetSegment(j);
            Serial.println("expire \n");
        }
    }

    // Turn of cellular if there are no measurements during sleepTimeout
    if (Time.now() - lastMeasurementTime > sleepTimeout && dataToSendArray[0].equals("") && Cellular.ready())
    {
        Serial.println("sleep");
        Particle.publish(statustopic, "sleeping");
        delay(1000);
        Cellular.off();
    }

    delay(10);
}


// Define interrupt methods
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

// Function that is called when an interrupt is fired
void debounceAndMeasure(int segment, int sensor)
{
    Serial.println("triggered");
    int index = 2 * segment + sensor;

    // Software debounce and register interrupt time
    if (millis() - lastInterruptTime[index] > 75)
    {
        Serial.println("triggered " + String(segment) + "," + String(sensor));
        velocityMeasure(segment, sensor);
        lastInterruptTime[index] = millis();
    }
}

// Process triggers to measure velocity
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

// Add measured velocity to data buffer
void sendVelocity(unsigned long timestamp, double velocity, int segment, int direction)
{
    int velocitySign = direction * 2 - 1; // determine the sign of the velocity
    dataToSend = String(timestamp) + "," + String(velocitySign * velocity) + "," + String(segment);

    // Add data to data buffer
    for (int i = 0; i < 128; i++)
    {
        if (dataToSendArray[i].equals(""))
        {
            dataToSendArray[i] = dataToSend;
            break;
        }
    }

    Serial.println(dataToSend);
}

// Check for data in data buffer and publish it to the cloud
void publishToCloud()
{
    String stringToSend;
    int amountToSend;
    while (true)
    {
        // If there is no connection, the connection will be restarted if 5 or more bikes are detected.
        if(!Cellular.ready() && !dataToSendArray[4].equals("")){ 
            Serial.println("trying to connect");
            Cellular.connect();
            unsigned long startConnecting = millis();
            while((!Cellular.ready()) && ((millis() - startConnecting) < connectTime) ){
            }
        }
        if (Cellular.ready()){
            if(!dataToSendArray[0].equals("")){ // if there is data available, send it
                Serial.println("sending data");
                stringToSend = "";
                amountToSend = 0;
                for (int i = 0; i < dataBufferSize; i++)
                { // Up to 9 strings from dataToSendArray are combined in stringToSend, seperated by a comma
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
                for (int i = 0; i < dataBufferSize - amountToSend; i++)
                { // dataToSendArray elements are shifed to the left
                    dataToSendArray[i] = dataToSendArray[i + amountToSend];
                }
                for (int i = dataBufferSize - amountToSend; i < dataBufferSize; i++)
                { // last places will be cleared
                    dataToSendArray[i] = "";
                }
                if (amountToSend > 0)
                {   
                    Particle.publish(datatopic, stringToSend);
                    delay(minimumTimeBetweenPublishes);
                } 
            }
        } 
        else 
        {
            Serial.println("waiting 10 secs before checking again");
            delay(10000);
        }
    }
}

// Reset a segment, called when a measurement expires or finishes
void resetSegment(int i)
{
    timestamp[i] = 0;
    firstSensor[i] = 0;
    frontWheelTime[i] = 0;
    rearWheelTime[i] = 0;
    frontWheelVelocity[i] = 0.0;
}
