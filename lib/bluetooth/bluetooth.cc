#include "Arduino.h"
#include "SoftwareSerial.h"

#include "bluetooth.h"

SoftwareSerial BT(RX_PIN, TX_PIN);

namespace bluetooth_ns
{
    unsigned long ms;

    void Setup()
    {
        BT.begin(9600);
        Serial.println("Bluetooth::initializing...");
        pinMode(13, OUTPUT);
        pinMode(RX_PIN, INPUT_PULLUP);
        pinMode(TX_PIN, OUTPUT);
        ms = millis();
        Serial.println("Bluetooth::done...");
    };

    void ReceiveData(Data *data, bool debug)
    {
        data->data = 0;
        data->received = false;
        if (BT.available())
        {
            int x = BT.read();
            if (debug)
            {
                Serial.print("Bluetooth::received ");
                Serial.println(x, DEC);
            }
            data->data = x;
            data->received = true;
        }
    }

    void SendData(SensorState *data[], byte data_len, bool debug)
    {
        if (debug)
        {
            Serial.print("Bluetooth::sent \n");
            for (byte ii = 0; ii < data_len; ++ii)
            {
                if (data[ii] != nullptr)
                {
                    Serial.print(data[ii]->sensor_name);
                    Serial.print(" ");
                    Serial.print(data[ii]->data, DEC);
                    Serial.print(" ");
                }
            }
            Serial.println();
        }
        for (byte ii = 0; ii < data_len; ++ii)
        {
            int buffer_size = 0;
            if (data[ii] != nullptr)
            {
                buffer_size += 2; // ':' and ';'
                buffer_size += 4; // sensor values goes from 0 to 1024
                buffer_size += data[ii]->sensor_name.length();
                char buffer[buffer_size];
                sprintf(buffer, "%s:%d;", data[ii]->sensor_name.c_str(), data[ii]->data);
                BT.write(buffer);
            }
        }
        BT.write("\n");
    }
}
