#include <Arduino.h>
#include "common.h"
#include "sound_detector.h"

namespace sound_detector
{
    int counter = 0;
    int average = 0;
    int averageDiff = 0;
    int sum = 0;
    int sumDiff = 0;
    int maxDiff = 0;
    unsigned long timer = 0;
    unsigned long int ignore_timer = 0;
    bool SOUND_IS_TRIGGERED = false;

    int registeredWave = 0;
    int direction = 0; // negative -> lowering tendency it has, positive -> it has rising tendency

    void reset(int currentMeasurement)
    {
        counter = 1;
        average = currentMeasurement;
        averageDiff = 0;
        sum = currentMeasurement;
        sumDiff = 0;
        maxDiff = 0;
        direction = 0;
        // timer = 0;
        // ignore_timer = 0;
    }

    void setup()
    {
        if (DEBUG_SOUND_DETECTOR)
        {
            Serial.begin(115200);
        }
        pinMode(SOUND_READING_PIN, INPUT);
    }

    bool check_sound(bool tweak_only, SensorState *state)
    {
        SOUND_IS_TRIGGERED = false;
        unsigned long int current_time = millis();
        if (timer > 0 && (current_time - timer) > MAX_WAIT)
        {
            timer = 0;
            registeredWave = 0;
            ignore_timer = 0;
            SOUND_IS_TRIGGERED = false;
            // digitalWrite(RELAY_PIN, LOW);
        }
        else
        {
            int currentMeasurement = (int)analogRead(SOUND_READING_PIN);

            state->data = currentMeasurement;

            sum += currentMeasurement;
            counter++;
            average = int(sum / counter);
            int currentDiff = abs(currentMeasurement - average);
            maxDiff = maxDiff > currentDiff ? maxDiff : currentDiff;
            sumDiff += currentDiff;
            averageDiff = int(sumDiff / counter);
            float LIMITER = average * .03;
            // float LIMITER = average * .07;
            // float LIMITER = average * .15;

            if (sum < 0 || sumDiff < 0 || counter < 0)
            {
                reset(currentMeasurement);
            }
            else if (!tweak_only && (ignore_timer == 0 || current_time - ignore_timer > DEAD_ZONE))
            {
                ignore_timer = 0;
                if (registeredWave > 0)
                {
                    direction = registeredWave - currentDiff;
                    registeredWave = currentDiff;
                }

                if ((float)currentDiff >= LIMITER && direction <= 0) // I dedect new wave or second with raising tendency
                {
                    if (DEBUG_FREQ)
                    {
                        Serial.print(currentMeasurement, DEC);
                        Serial.print(" ");
                        Serial.print(currentDiff, DEC);
                        Serial.print(" ");
                        Serial.print(average, DEC);
                        Serial.print(" ");
                        Serial.print(averageDiff, DEC);
                        Serial.print(" ");
                        Serial.println(maxDiff, DEC);
                    }

                    if (DEBUG_SOUND_DETECTOR)
                    {
                        // Serial.print(maxDiff, DEC);
                        // Serial.print(" ");
                        // Serial.println(currentDiff, DEC);
                        Serial.print("Timer ");
                        Serial.print(timer);
                        Serial.print(" current_time ");
                        Serial.println(current_time);
                    }
                    if (timer > 0)
                    {
                        if ((current_time - timer) < MAX_WAIT)
                        {
                            // digitalWrite(RELAY_PIN, HIGH);
                            SOUND_IS_TRIGGERED = true;
                        }
                        else
                        {
                            // digitalWrite(RELAY_PIN, LOW);
                            SOUND_IS_TRIGGERED = false;
                        }
                        if (DEBUG_SOUND_DETECTOR)
                        {
                            Serial.print(" SOUND_IS_TRIGGERED ");
                            Serial.println(SOUND_IS_TRIGGERED);
                        }
                        timer = 0;
                        registeredWave = 0;
                        // delayMicroseconds(15000);
                    }
                    else
                    {
                        timer = current_time;
                        registeredWave = currentDiff;
                        // delayMicroseconds(15000);
                    }
                    ignore_timer = millis();
                }
            }
        }
        return SOUND_IS_TRIGGERED;
    }
}