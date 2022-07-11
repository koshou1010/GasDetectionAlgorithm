#ifndef UTILS_H
#define UTILS_H
#define sensor_num 14
#define feature_num 10  //len of feature num must same as enum 


enum FeatureShortName{rmdr, rsdr, rms, r5dr, r10dr, r5s, r10s, rmdr_nor, r5dr_nor, r10dr_nor};

struct ReturnInfo
{
  const char *algo_version = "0.1.2";
  char time_buffer[32];
  int sys_valve_status; // purge = 1, sample = 0; default = 0
  int algo_valve_status; // purge = 1, sample = 0; default = 0
  int alarm_status; // alarm = 1, no_alarm = 0; default = 0
  int predict_label; // algorithm predict label of result, e.g., 0, 1, 2, default -1
  const char *predict_labelname; // algorithm predict labelname of result, e.g., DMMP, MS, TIC, default None
  int status; //algorithm judgment status, use to log more cleaner, default 0
  float feature_set[feature_num][sensor_num] = {0};
  const char *feature_set_name [feature_num] = {"rmdr", "rsdr", "rms", "r5dr", "r10dr", "r5s", "r10s", "rmdr_nor", "r5dr_nor", "r10dr_nor"};
};



#endif