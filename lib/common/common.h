#ifndef COMMON
#define COMMON

#define PIR_PIN 11
#define SOUND_READING_PIN A0
#define RELAY_PIN 3
#define ANALOG_SENSORS_CNT 1

// BT_TRANSLATIONS
#define BT_DISABLE_SENSORS 0x9B
#define BT_ENABLE_SENSORS 0x9A

#define BT_DISABLE_PIR 0x99
#define BT_ENABLE_PIR 0x98

#define BT_DISABLE_SOUND 0x97
#define BT_ENABLE_SOUND 0x96

#define BT_TOGGLE_LIGHT 0x01

#define BT_SEND_CURRENT_ANALOG_SENSOR_DATA 0x02
#define BT_SEND_CURRENT_APP_STATE 0x03

struct Data
{
    bool received;
    unsigned long data;
};

struct SensorState
{
    String sensor_name;
    int data;
};

#endif