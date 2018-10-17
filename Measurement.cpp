class Measurement
{
  public:
    long TriggerTime;
    long TriggerMillis[4];
    int count = 0;

    Measurement()
    {
        this.TriggerTime = millis();
        this.addTime;
    }

    void addTime()
    {
        if (count < 4)
        {
            TriggerMillis[count] = millis();
        }
    }
}