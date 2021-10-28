#include <Arduino.h>
#include "common.h"
#include "sound_detector.h"
#include "pir_detector.h"
#include "bluetooth.h"

#define DEBUG false
#define SOUND_DEAD_ZONE_MS 2000

int TOGGLE_VALUE = LOW;

bool LAST_MOTION_VALUE = false;

unsigned long int sound_ignore_timer = 0;
bool ignore_all = false;
bool ignore_motion = false;
bool ignore_sound = true; // sound is broken now

SensorState sound_sensor = {"sound", 0};

int toggle()
{
  TOGGLE_VALUE = TOGGLE_VALUE == LOW ? HIGH : LOW;
  return TOGGLE_VALUE;
}

Data bluetooth_data = {false, 0};

void setup()
{
  if (DEBUG)
  {
    Serial.begin(115200);
  }
  pinMode(PIR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SOUND_READING_PIN, INPUT);
  digitalWrite(RELAY_PIN, LOW);
  bluetooth_ns::Setup();
}

void loop()
{
  bluetooth_ns::ReceiveData(&bluetooth_data, DEBUG);

  if (bluetooth_data.received)
  {
    if (bluetooth_data.data == BT_DISABLE_SENSORS || bluetooth_data.data == BT_ENABLE_SENSORS)
    {
      ignore_all = bluetooth_data.data == BT_DISABLE_SENSORS;
    }
    else if (bluetooth_data.data == BT_DISABLE_PIR || bluetooth_data.data == BT_ENABLE_PIR)
    {
      ignore_motion = bluetooth_data.data == BT_DISABLE_PIR;
    }
    else if (bluetooth_data.data == BT_DISABLE_SOUND || bluetooth_data.data == BT_ENABLE_SOUND)
    {
      ignore_sound = bluetooth_data.data == BT_DISABLE_SOUND;
    }
    else if (bluetooth_data.data == BT_TOGGLE_LIGHT)
    {
      digitalWrite(RELAY_PIN, toggle());
    }
    else if (bluetooth_data.data == BT_SEND_CURRENT_ANALOG_SENSOR_DATA)
    {
      SensorState *data[ANALOG_SENSORS_CNT] = {&sound_sensor};
      bluetooth_ns::SendData(data, ANALOG_SENSORS_CNT, DEBUG);
    }
    else if (bluetooth_data.data == BT_SEND_CURRENT_APP_STATE)
    {
      SensorState sound = {"sound", !ignore_sound};
      SensorState motion = {"motion", !ignore_motion};
      SensorState main_control = {"ignoreSensors", ignore_all};
      SensorState light = {"state", TOGGLE_VALUE};

      SensorState *data[4] = {&sound, &motion, &main_control, &light};
      bluetooth_ns::SendData(data, 4, DEBUG);
    }
  }

  if (!ignore_all)
  {
    unsigned long int current_timer = millis();

    if (!ignore_sound)
    {
      const bool only_tweak_sound = sound_ignore_timer != 0 && current_timer - sound_ignore_timer <= SOUND_DEAD_ZONE_MS;
      sound_ignore_timer = only_tweak_sound ? sound_ignore_timer : 0;
      bool sound_detected = sound_detector::check_sound(only_tweak_sound, &sound_sensor);
      if (sound_detected)
      {
        if (DEBUG)
        {
          Serial.println("sound detected");
        }
        sound_ignore_timer = millis();
        digitalWrite(RELAY_PIN, toggle());
      }
    }

    if (!ignore_motion)
    {
      bool motion_detected = motion_detector::check_motion();
      if (motion_detected && LAST_MOTION_VALUE == false && TOGGLE_VALUE == LOW)
      {
        if (DEBUG)
        {
          Serial.println("motion detected");
        }
        digitalWrite(RELAY_PIN, toggle());
      }
      LAST_MOTION_VALUE = motion_detected;
    }
  }
}