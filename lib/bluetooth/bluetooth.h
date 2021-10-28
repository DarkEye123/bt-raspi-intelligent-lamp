
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define RX_PIN 4 // TX on BT module
#define TX_PIN 5 // RX on BT module

#include "common.h"

namespace bluetooth_ns
{
    void Setup();
    void ReceiveData(Data *data, bool debug);
    void SendData(SensorState *data[], byte data_len, bool debug);
}

#endif