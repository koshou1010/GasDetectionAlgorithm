#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <string>
#include <math.h>
#include <iomanip>
#include "sensor_struct.h"
#include "algo.h"
#include "utils.h"
#include "parameters.h"
#include "svm.h"
#include "classifier.h"

using namespace std;

int PMODE = 1; // 0 : debug mode, 1 : mbed mode
ParameterSet parameter_set;
ReturnInfo return_info;


extern Sensor Sensor_S1;
extern Sensor Sensor_S2;
extern Sensor Sensor_S3;
extern Sensor Sensor_S4;
extern Sensor Sensor_S5;
extern Sensor Sensor_S6;
extern Sensor Sensor_S7;
extern Sensor Sensor_S8;
extern Sensor Sensor_S9;
extern Sensor Sensor_S10;
extern Sensor Sensor_S11;
extern Sensor Sensor_S12;
extern Sensor Sensor_S13;
extern Sensor Sensor_S14;



int conti_times = 0;
int counter_num = 0; // use for check if wait num at least --> status1
int sensor_voting = 0;
int gasin_counter = 0; // use to flag earlier features when gas in 
int stable_counter = 0;
int purge_counter = 0;
bool reaction_stable_flag = false; // if judge reaction stable, its true
int counter_ary[3][sensor_num] = {0}; // enum, if sucess threshold, +=1 
int voting_ary[3][sensor_num] = {0}; // eunm, use for judgment each sensor sucess threshold or not, if sucess will be change 0 to 1
float judgment_ary[judgment_row][sensor_num] = {0}; // use to save value to calculate parameter and then judgment status
float difference_rate_ary[sensor_num] = {0}; // store the parameters of all sensor, then can jugde status
float baseline_tmp_ary[baseline_length][sensor_num] = {0};
float baseline_avg[sensor_num] = {0};
float rmdr_ary[sensor_num] = {0};
float rsdr_ary[sensor_num] = {0};
float rms_ary[sensor_num] = {0};
float r5dr_ary[sensor_num] = {0};
float r10dr_ary[sensor_num] = {0};
float r5s_ary[sensor_num] = {0};
float r10s_ary[sensor_num] = {0};
float rmdr_nor_ary[sensor_num] = {0};
float r5dr_nor_ary[sensor_num] = {0};
float r10dr_nor_ary[sensor_num] = {0};
bool rmdr_flag = false;
bool rms_flag = false;
bool features_done = false; // if features extraction done, its true
bool printed_feature = false; // if printed, its true
bool printed_classifier = false;
bool classifier_done = false; 
/* only test use*/
int cnter = 0;
/*only test use end*/

// use for quicklly switch debug or mbed of print func
void myprintf(const char *format, ...)
{
    char buffer[80];
    if (PMODE == 0)
    {
        va_list aptr;
        int ret;
        va_start(aptr, format);
        ret = vsprintf(buffer, format, aptr);
        va_end(aptr);
        printf("%s", buffer);
    }
    if (PMODE == 1)
    {
    }
}

//store value in judgment_ary which will be calculate parameter 
void store_judgment_array(float sensor_value[sensor_num])
{
    // shifft
    for (int i = 0; i < sensor_num; i++)
    {
        judgment_ary[0][i] = judgment_ary[1][i];
    }
    // push back
    for (int i = 0; i < sensor_num; i++)
    {
        judgment_ary[1][i] = sensor_value[i];
    }
    // myprintf("judge, ");
    // for(int k = 0; k < judgment_row; k++)
    // {
    //     for(int i = 0; i < sensor_num; i++)
    //     {
    //         myprintf("%.5f,", judgment_ary[k][i]);
    //     }
    //     myprintf("\r\n");
    // }
    // myprintf("\r\n");
}

// calculate each two sec data, (Rt2 - Rt1))/Rt1, and store in difference_rate_ary
void calculate_judgment_array(float sensor_value[sensor_num])
{
    for (int i = 0; i < sensor_num; i++)
    {
        difference_rate_ary[i] = (judgment_ary[1][i] - judgment_ary[0][i])/judgment_ary[0][i];
    }
    // myprintf("para, ");
    // for (int i = 0; i < sensor_num; i++)
    // {
    //     myprintf("%.5f,", difference_rate_ary[i]);
    // }
    // myprintf("\r\n");
}




// rmdr 
void resistance_max_difference_rate(float sensor_value[sensor_num])
{
    if(!rmdr_flag)
    {
        for(int i = 0; i < sensor_num; i++)
        {
            rmdr_ary[i] = (sensor_value[i] - baseline_avg[i]) / baseline_avg[i];
        }
        // for(int i = 0; i < sensor_num; i++)
        // {
            // myprintf("%.3f, ", rmdr_ary[i]);
        // }
        // myprintf("\n");
        rmdr_flag = true;
    }
    else
    {
        for(int i = 0; i < sensor_num; i++)
        {
            if(fabs((sensor_value[i] - baseline_avg[i]) / baseline_avg[i]) > fabs(rmdr_ary[i]))
            {
                rmdr_ary[i] = (sensor_value[i] - baseline_avg[i]) / baseline_avg[i]; 
            }
        }
    }
    for(int i = 0; i < sensor_num; i++)
    {
        return_info.feature_set[rmdr][i] = rmdr_ary[i];
    }
    
}

// rsdr
/*  reaction stable difference rate, each sensor get theirself stable value
    if this sensor no stable or stable contiunte time not enough, grab all status 3 value(reaction stable)
    if this sensor has two times stable status, choose the last status
    if didnt judge statble status, replace with resistance max difference rate*/
void resistance_stable_difference_rate(float sensor_value[sensor_num])
{
    if(reaction_stable_flag)
    {

    }
    else
    {
        for (int i = 0; i < sensor_num ; i++)
        {
            rsdr_ary[i] = rmdr_ary[i];
        }
    }
    for(int i = 0; i < sensor_num; i++)
    {
        return_info.feature_set[rsdr][i] = rsdr_ary[i];
    }

}

// rms
void resistance_max_slope(float sensor_value[sensor_num])
{

    if(!rms_flag)
    {
        for (int i = 0; i < sensor_num; i++)
        {
            rms_ary[i] = (judgment_ary[1][i] - judgment_ary[0][i])/1 ;   //Suppose the interval between 2 data is one second 
        }
        // for(int i = 0; i < sensor_num; i++)
        // {
        //     myprintf("%.3f, ", rms_ary[i]);
        // }
        // myprintf("\n");
        rms_flag = true;
    }
    else
    {
        for(int i = 0; i < sensor_num; i++)
        {
            if(fabs((judgment_ary[1][i] - judgment_ary[0][i])/1) > fabs(rms_ary[i]))
            {
                rms_ary[i] = (judgment_ary[1][i] - judgment_ary[0][i])/1; 
            }
        }
    }
    for(int i = 0; i < sensor_num; i++)
    {
        return_info.feature_set[rms][i] = rms_ary[i];
    }
}

// r5dr
void resistance_5s_difference_rate(float sensor_value[sensor_num])
{
    if(gasin_counter == 5)
    {
        for (int i = 0; i < sensor_num; i++)
        {
            r5dr_ary[i] = (sensor_value[i] - baseline_avg[i]) / baseline_avg[i]; 
        }
        for(int i = 0; i < sensor_num; i++)
        {
            return_info.feature_set[r5dr][i] = r5dr_ary[i];
        }
    }
}

// r10dr
void resistance_10s_difference_rate(float sensor_value[sensor_num])
{
    if(gasin_counter == 10)
    {
        for (int i = 0; i < sensor_num; i++)
        {
            r10dr_ary[i] = (sensor_value[i] - baseline_avg[i]) / baseline_avg[i]; 
        }
        for(int i = 0; i < sensor_num; i++)
        {
            return_info.feature_set[r10dr][i] = r10dr_ary[i];
        }
    }
}

// r5s
void resistance_5s_slope(float sensor_value[sensor_num])
{
    if(gasin_counter == 5)
    {
        for (int i = 0; i < sensor_num ;i++)
        {
            r5s_ary[i] = (judgment_ary[1][i] - judgment_ary[0][i])/1 ;   //Suppose the interval between 2 data is one second 
        }
        for(int i = 0; i < sensor_num; i++)
        {
            return_info.feature_set[r5s][i] = r5s_ary[i];
        }
    }
}

// r10s
void resistance_10s_slope(float sensor_value[sensor_num])
{
    if(gasin_counter == 10)
    {
        for (int i = 0; i < sensor_num ;i++)
        {
            r10s_ary[i] = (judgment_ary[1][i] - judgment_ary[0][i])/1 ;   //Suppose the interval between 2 data is one second 
        }
        for(int i = 0; i < sensor_num; i++)
        {
            return_info.feature_set[r10s][i] = r10s_ary[i];
        }
    }
}

// print all feature
void print_all_feature()
{

	myprintf("\n");
    for (int i = 0; i < feature_num; i++)
    {
        myprintf("features,%s,", return_info.feature_set_name[i]);
        for (int k = 0; k < sensor_num; k++)
        {
            myprintf("%.5f, ", return_info.feature_set[i][k]);
        }
        myprintf("\n");
    }
    myprintf("\r\n");
    printed_feature = true;
}


void do_normalize(float raw_ary[sensor_num], float normalize_ary[sensor_num], FeatureShortName shortname)
{
    float sum = 0.0;
    for(int i = 0; i < sensor_num; i++)
    {
        sum += raw_ary[i];
    }
    for(int k = 0; k < sensor_num; k++)
    {
        normalize_ary[k] = raw_ary[k]/sum;
    }
    for(int j = 0; j < sensor_num; j++)
    {
        return_info.feature_set[shortname][j] = normalize_ary[j];
    }
}

// get all feature 
void feature_extraction(float sensor_value[sensor_num])
{
    /*feature define (short name)*/
    /*
    1.	(Reaction max – baseline)/baseline   (rmdr)
    2.	(Stable_avg – baseline) / baseline   (rsdr)
    3.	(max_slope)                          (rms)
    4.	(Reaction 5s – baseline)/baseline    (r5dr)
    5.	(Reaction 10s – baseline)/baseline   (r10dr)
    6.	(slope in (5s-4s)/1s)                (r5s)
    7.	(slope in (10s-9s)/1s)               (r10s)
    */

    if(return_info.status == 0 || return_info.status == 1)
    {
        float baseline_sum[sensor_num] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        // baseline_length-parameter_set.conti_array[1] avoid grab gas in value
        for(int k = 0; k < baseline_length-parameter_set.conti_array[gas_in]; k++)
        {
            for(int i = 0; i < sensor_num; i++)
            {
                baseline_sum[i] += baseline_tmp_ary[k][i];
            }
        }
        for (int i = 0; i < sensor_num; i++)
        {
            baseline_avg[i] = baseline_sum[i] / (baseline_length-parameter_set.conti_array[1]);
        }
        // myprintf("baseline_avg, ");
        // for (int i = 0; i < sensor_num; i++)
        // {
        //     myprintf("%.5f,", baseline_avg[i]);
        // }
        // myprintf("\r\n");
    }
    if(return_info.status == 2 || return_info.status == 3)
    {
        resistance_max_difference_rate(sensor_value);
        resistance_max_slope(sensor_value);
        resistance_5s_difference_rate(sensor_value);
        resistance_10s_difference_rate(sensor_value);
        resistance_5s_slope(sensor_value);
        resistance_10s_slope(sensor_value);
    }
    if(return_info.status == 4)
    {
        resistance_stable_difference_rate(sensor_value);
    }

    do_normalize(rmdr_ary, rmdr_nor_ary, rmdr_nor);
    do_normalize(r5dr_ary, r5dr_nor_ary, r5dr_nor);
    do_normalize(r10dr_ary, r10dr_nor_ary, r10dr_nor);

}

// store value in baseline_tmp_ary at status 0 and 1 
void store_baseline_value(float sensor_value[sensor_num])
{
    // shifft
    for (int k = 1; k < baseline_length; k++)
    {
        for (int i = 0; i < sensor_num; i++)
        {
            baseline_tmp_ary[k-1][i] = baseline_tmp_ary[k][i];
        }
    }
    // push back
    for (int i = 0; i < sensor_num; i++)
    {
        baseline_tmp_ary[baseline_length-1][i] = sensor_value[i];
    }
    // myprintf("baseline, ");
    // for(int k = 0; k < baseline_length; k++)
    // {
    //     for(int i = 0; i < sensor_num; i++)
    //     {
    //         myprintf("%.5f,", baseline_tmp_ary[k][i]);
    //     }
    //     myprintf("\r\n");
    // }
    // myprintf("\r\n");
}





// return sum of each sensor voting result
int calculate_voting_result(int status)
{
    int sum = 0;
    for(int i = 0; i < sensor_num; i++)
    {
        sum += voting_ary[status][i];
    }
    return sum;
}

// judge current status and do the action at each status
void judgment_status_and_action(float sensor_value[sensor_num])
{
    /* Status Defined
    status 0: wait baseline stable
    status 1: baseline stable
    status 2: gas in
    status 3: reaction stable
    status 4: wait system switch
    status 9: purge
    */

    /******************judge status 1(baseline_stable)*********************/
    if (return_info.status == 0)
    {
        store_baseline_value(sensor_value);
        for (int i = 0; i < sensor_num; i++)
        {
            if(fabs(difference_rate_ary[i]) <= parameter_set.threshold_array[baseline_stable][i])
            {
                counter_ary[baseline_stable][i]++;
            }
            else
            {
                counter_ary[baseline_stable][i] = 0;
                voting_ary[baseline_stable][i] = 0;
            }
            if(counter_ary[baseline_stable][i] >= parameter_set.conti_times)
            {
                voting_ary[baseline_stable][i] = 1;
            }
            // printf("%i, %i, ", counter_ary[i], voting_ary[i]);
        }
        // printf("\r\n");
        int voting_result = calculate_voting_result(baseline_stable);
        if(voting_result >= parameter_set.judgment_sensor_num && counter_num == parameter_set.at_least_num)
        {
            return_info.status = 1;
            // printf("status = %i ", return_info.status);
        }
    }
    /******************judge status 2(gas_in)*********************/
    else if (return_info.status == 1)
    {
        store_baseline_value(sensor_value);
        for (int i = 0; i < sensor_num; i++)
        {
            if(fabs(difference_rate_ary[i]) >= parameter_set.threshold_array[gas_in][i])
            {
                counter_ary[gas_in][i]++;
            }
            else
            {
                counter_ary[gas_in][i] = 0;
                voting_ary[gas_in][i] = 0;
            }
            if(counter_ary[gas_in][i] >= parameter_set.conti_times)
            {
                voting_ary[gas_in][i] = 1;
            }
            // printf("%.5f, %.3f, ", fabs(difference_rate_ary[i]), parameter_set.threshold_array[gas_in][i]);
            // printf("%i, %i, ", counter_ary[gas_in][i], voting_ary[gas_in][i]);
        }
        // printf("\r\n");
        int voting_result = calculate_voting_result(gas_in);
        // printf("%i \n", voting_result);
        if(voting_result >= parameter_set.judgment_sensor_num)
        {
            return_info.status = 2;
            // printf("status = %i ", return_info.status);
        }
    }
    /******************judge status 3(reaction_stable)*********************/
    else if (return_info.status == 2)
    {
        gasin_counter ++;
        for (int i = 0; i < sensor_num; i++)
        {
            if(fabs(difference_rate_ary[i]) <= parameter_set.threshold_array[reaction_stable][i])
            {
                counter_ary[reaction_stable][i]++;
            }
            else
            {
                counter_ary[reaction_stable][i] = 0;
                voting_ary[reaction_stable][i] = 0;
            }
            if(counter_ary[reaction_stable][i] >= parameter_set.conti_times)
            {
                voting_ary[reaction_stable][i] = 1;
            }
            // printf("%.5f, %.5f, ", fabs(difference_rate_ary[i]), parameter_set.threshold_array[gas_in][i]);
            // printf("%i, %i, ", counter_ary[i], voting_ary[i]);
        }
        // printf("\r\n");
        int voting_result = calculate_voting_result(reaction_stable);
        // printf("%i \n", voting_result);
        if(voting_result >= parameter_set.judgment_sensor_num)
        {
            return_info.status = 3;
            reaction_stable_flag = true;
            // printf("status = %i ", return_info.status);
        }
    }
    else if (return_info.status == 3)
    {
        gasin_counter ++;
        stable_counter++;
        if(stable_counter > parameter_set.stable_limit)
        {
            features_done = true;
            if(classifier_done)
            {
                return_info.status = 4; 
            }
        //    printf("status = %i ", return_info.status);
        }
    }
    else if (return_info.status == 4)
    {

        // if system switch purge then status = 9
        return_info.algo_valve_status = 1;
        if(return_info.sys_valve_status == 1 && return_info.alarm_status == 1)
        {
            return_info.status  = 9;
        }
    }
    else if (return_info.status == 9)
    {
        purge_counter ++;
        if(purge_counter >= parameter_set.purge_limit)
        {
            return_info.algo_valve_status = 0;
        }
        if(return_info.sys_valve_status == 0)
        {
            return_info.status  = 0;
        }
    }
    feature_extraction(sensor_value);
    if(features_done)
    {
        if(!printed_feature)
        {
            print_all_feature();
            if(!classifier_done)
            {
                ClassifierSVM csvm;
                svm_model *modelh = load_model(csvm);
                csvm.svm_run(modelh);
                svm_free_and_destroy_model(&modelh);
                classifier_done = true;
            }
        }
    }
}


void logger_message(float sensor_value[sensor_num])
{
        // print message on chip use for debugging
        printf("data,status=%i,", return_info.status);
        for (int i = 0; i < sensor_num; i++)
        {
                printf("%.5f,", sensor_value[i]);
        }
        printf("res_difference_rate,");
        for (int k = 0; k < sensor_num; k++)
        {
                printf("%.5f,", difference_rate_ary[k]);
        }
        printf("counter_ary,");
        if (return_info.status == 0 || return_info.status == 1 || return_info.status == 2)
        {
                for (int j = 0; j < sensor_num; j++)
                {
                        printf("%i,", counter_ary[return_info.status][j]);
                }
                printf("voting_ary,");
                for (int l = 0; l < sensor_num; l++)
                {
                        printf("%i,", voting_ary[return_info.status][l]);
                }
        }
        else
        {
                for (int j = 0; j < sensor_num; j++)
                {
                        printf("0,");
                }
                printf("voting_ary,");
                for (int l = 0; l < sensor_num; l++)
                {
                        printf("0,");
                }
        }
        printf("stable_counter,%i", stable_counter);
        printf("purge_counter,%i", purge_counter);
        if (features_done)
        {
                if (!printed_feature)
                {
                        printf("\n");
                        for (int i = 0; i < feature_num; i++)
                        {
                                printf("features,%s,", return_info.feature_set_name[i]);
                                for (int k = 0; k < sensor_num; k++)
                                {
                                        printf("%.3f, ", return_info.feature_set[i][k]);
                                }
                                printf("\n");
                        }
                        printed_feature = true;
                }
                if (!printed_classifier)
                {
                        printf("classifier,svm,%s", return_info.predict_labelname);
                        printed_classifier = true;
                }
        }
        printf("\n");
}

// entrance of algorithm return status
int algo_return()
{
    float sensor_value[sensor_num] = {Sensor_S1.AV_RS, Sensor_S2.AV_RS, Sensor_S3.AV_RS, Sensor_S4.AV_RS, Sensor_S5.AV_RS, Sensor_S6.AV_RS, Sensor_S7.AV_RS, Sensor_S8.AV_RS, Sensor_S9.AV_RS, Sensor_S10.AV_RS, Sensor_S11.AV_RS, Sensor_S12.AV_RS, Sensor_S13.AV_RS, Sensor_S14.AV_RS};
    // avoid heap overflow
    if(counter_num < parameter_set.at_least_num)
    {
        counter_num++;
    }
    myprintf("status=%i,", return_info.status);
    for (int i = 0; i < sensor_num; i++)
    {
        myprintf("%.5f,", sensor_value[i]);
    }
    myprintf("\n");
    store_judgment_array(sensor_value);
    calculate_judgment_array(sensor_value);
    judgment_status_and_action(sensor_value);

    logger_message(sensor_value);

    return return_info.status;
}

// reset all variable as system need to reset
void algo_reset()
{
    return_info.status = 0;
    return_info.sys_valve_status = 0;
    return_info.algo_valve_status = 0;
    return_info.alarm_status = 0;
    return_info.predict_label = -1;
    return_info.predict_labelname = "None";

    cnter = 0;
    conti_times = 0;
    counter_num = 0;
    gasin_counter = 0;
    stable_counter = 0;
    purge_counter = 0;
    reaction_stable_flag = false;
    rmdr_flag = false;
    rms_flag = false;
    features_done = false;
    printed_feature = false;
    printed_classifier = false;
    classifier_done = false;
    for (int i = 0; i < sensor_num; i++)
    {
        difference_rate_ary[i] = 0;
        baseline_avg[i] = 0;
        rmdr_ary[i] = 0;
        rsdr_ary[i] = 0;
        rms_ary[i] = 0;
        r5dr_ary[i] = 0;
        r10dr_ary[i] = 0;
        r5s_ary[i] = 0;
        r10s_ary[i] = 0;
        rmdr_nor_ary[i] = 0;
        r5dr_nor_ary[i] = 0;
        r10dr_nor_ary[i] = 0;
        for (int j = 0; j < 3; j++)
        {
            counter_ary[j][i] = 0;
            voting_ary[j][i] = 0;
        }
        for (int m = 0; m < judgment_row; m++)
        {
            judgment_ary[m][i] = 0;
        }
        for (int k = 0; k < baseline_length; k ++)
        {
            baseline_tmp_ary[k][i] = 0;
        }
        for (int l = 0; l < feature_num; l++)
        {
            return_info.feature_set[l][i] = 0;
        }
    }
}