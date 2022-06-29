#ifndef PARAMETERS_H
#define PARAMETERS_H
#define judgment_row 2 // decise the row of judgment_ary
#define baseline_length 30  // if I set 30, then I will calculate average of 30-conti_array[1], avoid grab gas in value 


enum {baseline_stable, gas_in, reaction_stable};
struct ParameterSet
{
    int purge_limit = 30;
    int at_least_num = 30; // use for check if wait num at least --> status1
    int stable_limit = 30; // stable limit
    int judgment_sensor_num = 6; 
    int conti_times = 5; // how long sucess threshold then judge to next status
    // enumerate
    float threshold_array[3][sensor_num] = {
        {0.0006, 0.0006, 0.0006, 0.0006, 0.0006, 0.0006,  0.0006, 0.0006, 0.0006, 0.0006, 0.0006, 0.0006, 0.0006, 0.0006}, //baseline_stable
        {0.009, 0.008,  0.015, 0.0006, 0.0025, 0.0009,  0.0015, 0.001,  0.0015, 0.0012, 0.002, 0.02,  0.0012, 0.001}, //gas_in 
        {0.0033, 0.005, 0.015,  0.0006, 0.0025, 0.0009,  0.0015,  0.001, 0.0015, 0.0012, 0.002,  0.02,  0.0012, 0.001}  //reaction_stable
    };

    float conti_array[3] = {10,8,8}; // enumerate, each status of conti how many times; index 0 : baseline stable, index 1 : gas in, index 2 : reaction stable
};





#endif