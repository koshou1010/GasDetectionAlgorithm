#ifndef SENSOR_STRUCT_H
#define SENSOR_STRUCT_H
#include <string>
using namespace std;

typedef struct
{
        string id;
        float AFE_reading; // from sensor reading
        float ADC_revert;  // parameter for recovery sensor reading from ADC output
        float RS;
        float AV_RS;
        string unit;        // for sensor unit
        float sensor_value; // for sensor value
        int transmit_flag;  // for determine whether to output data
} Sensor;

typedef struct
{
        int windowcount;
        int window;
} average;




#endif
