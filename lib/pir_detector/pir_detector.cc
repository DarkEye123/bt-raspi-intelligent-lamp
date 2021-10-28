#include <Arduino.h>
#include "common.h"
#include "pir_detector.h"

namespace motion_detector
{
    long unsigned int lowIn;
    boolean lockLow = true;
    boolean takeLowTime;
    int PIRValue = 0;
    unsigned long int ignore_timer = 0;
    bool MOTION_IS_TRIGGERED = false;
    bool check_motion()
    {
        unsigned long int current_time = millis();
        if (ignore_timer == 0 || current_time - ignore_timer > PIR_IGNORE_WAIT)
        {
            ignore_timer = 0;
            if (digitalRead(PIR_PIN) == HIGH)
            {
                if (lockLow)
                {
                    PIRValue = 1;
                    lockLow = false;
                    if (PIR_DEBUG)
                    {
                        Serial.println("Motion detected.");
                    }
                    // digitalWrite(RELAY_PIN, HIGH);
                    MOTION_IS_TRIGGERED = true;
                    // delay(50);
                    ignore_timer = millis();
                }
                takeLowTime = true;
            }
            if (digitalRead(PIR_PIN) == LOW)
            {
                if (takeLowTime)
                {
                    lowIn = millis();
                    takeLowTime = false;
                }
                if (!lockLow && millis() - lowIn > PIR_MAX_WAIT)
                {
                    PIRValue = 0;
                    lockLow = true;
                    if (PIR_DEBUG)
                    {
                        Serial.println("Motion ended.");
                    }
                    // digitalWrite(RELAY_PIN, LOW);
                    MOTION_IS_TRIGGERED = false;
                    // delay(50);
                    ignore_timer = millis();
                }
            }
        }
        return MOTION_IS_TRIGGERED;
    }
}
