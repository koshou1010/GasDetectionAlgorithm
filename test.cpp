#include <stdio.h>
#include <string.h>
#include <io.h>
#include <direct.h>

#include <iostream>
#include <fstream>
#include <string>
#include "algo.h"
#include "sensor_struct.h"
#include "utils.h"


using namespace std;
extern ReturnInfo return_info;
Sensor Sensor_S1 = {"S1", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S2 = {"S2", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S3 = {"S3", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S4 = {"S4", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S5 = {"S5", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S6 = {"S6", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S7 = {"S7", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S8 = {"S8", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S9 = {"S9", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S10 = {"S10", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S11 = {"S11", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S12 = {"S12", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S13 = {"S13", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};
Sensor Sensor_S14 = {"S14", 2.000, 0.4, 50.1, 50.5, "kohm", 1, 1};

string sensor_value[sensor_num]; // use to save Rs value
int count_pos = 0;
int result = 0;
/*split str from main.cpp data and pushback in vector "sensor_value"*/
void string_to_array(string s)
{
	// cout << s << endl;
	int current = 0; // from first space start
	int next;
	while (1)
	{
		next = s.find_first_of("[,] ", current);
		if (next != current)
		{
			string tmp = s.substr(current, next - current);
			if (tmp.size() != 0) // ignore blank string
			{
				sensor_value[count_pos] = tmp;
				// cout << count_pos << " , "<< tmp <<endl;
				count_pos = count_pos + 1;
			}
			if (tmp.size() == 0)
			{
				count_pos = 0;
			}
		}
		if (next == string::npos)
		{
			break;
		}
		current = next + 1; // start from next+1
	}
}

int main()
{

	ifstream ifs;
	char buffer[512] = {0};

	ifs.open(".\\dataset_data\\task__SXT273W5NHA__2022-04-20T17-18-01.788.txt");
	algo_reset();

	// printf("%i\r\n",return_info->purge);
	// printf("%i\r\n",return_info->return_purge);
	// printf("%i\r\n",return_info->purge_flag);
	// printf("%i\r\n",return_info->sample);
	// printf("%i\r\n",return_info->return_sample);
	// printf("%i\r\n",return_info->sample_flag);
	// printf("%i\r\n",return_info->predict_label);
	// printf("%s\r\n",return_info->predict_labelname);
	// printf("%i\r\n",return_info->status);

	if (!ifs.is_open())
	{
		cout << "Failed to open file.\n";
	}
	else
	{
		while (!ifs.eof())
		{
			ifs.getline(buffer, sizeof(buffer));
			string s(buffer);
			// cout << s << endl;
			string_to_array(s);
			Sensor_S1.AV_RS = std::stof(sensor_value[0]);
			Sensor_S2.AV_RS = std::stof(sensor_value[1]);
			Sensor_S3.AV_RS = std::stof(sensor_value[2]);
			Sensor_S4.AV_RS = std::stof(sensor_value[3]);
			Sensor_S5.AV_RS = std::stof(sensor_value[4]);
			Sensor_S6.AV_RS = std::stof(sensor_value[5]);
			Sensor_S7.AV_RS = std::stof(sensor_value[6]);
			Sensor_S8.AV_RS = std::stof(sensor_value[7]);
			Sensor_S9.AV_RS = std::stof(sensor_value[8]);
			Sensor_S10.AV_RS = std::stof(sensor_value[9]);
			Sensor_S11.AV_RS = std::stof(sensor_value[10]);
			Sensor_S12.AV_RS = std::stof(sensor_value[11]);
			Sensor_S13.AV_RS = std::stof(sensor_value[12]);
			Sensor_S14.AV_RS = std::stof(sensor_value[13]);

			// cout << Sensor_S1.value << " , " << Sensor_S2.value << " , " << Sensor_S3.value << " , " << Sensor_S4.value << " , " << Sensor_S5.value << " , " << Sensor_S6.value << endl;
			result = algo_return();

			// printf("%s,%i",return_info.svm_predict_labelname,return_info.svm_predict_label);
			// printf("%s,%i",return_info.knn_predict_labelname,return_info.knn_predict_label);
			// printf("%i,%i,%i,%i,%s",return_info.status,return_info.algo_valve_status,return_info.sys_valve_status,return_info.predict_label,return_info.predict_labelname);

			ifs.get();
			if (ifs.peek() == '\n')
				break;
		}
		// save_status(file_list[i]);
		// save_features("./data/task__AU03WYGQA__2021-12-04T14-44-05.017.ndjson.csv.txt");
		ifs.close();
	}

	return 0;
}
