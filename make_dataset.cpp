#include <iostream>
#include <string> 
#include <fstream> 
#include <io.h>
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
struct _finddata_t fileinfo;
string in_path;
string in_name;
string sensor_value[sensor_num]; // use to save Rs value
int count_pos = 0;
int result = 0;


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

void print_filename()
{
    ifstream fin(in_name);
    if (!fin)
    {
        cerr << "open file error" << endl;
        exit(-1);
    }
    cout << in_name<< endl;
}


int main()
{
	in_path = ".\\dataset_data";
	string curr = in_path + "\\*.txt";
	long handle;
	if ((handle = _findfirst(curr.c_str(), &fileinfo)) == -1L)
	{
		cout << "not find file!" << endl;
		return 0;
	}
    else
    {
        in_name = in_path + "\\" + fileinfo.name;
        while (!(_findnext(handle, &fileinfo)))
        {
            in_name = in_path + "\\" + fileinfo.name;
			ifstream ifs;
			char buffer[512] = {0};
			ifs.open(in_name);
			print_filename();
			algo_reset();
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
					result = algo_return();
					ifs.get();
					if (ifs.peek() == '\n')
						break;
				}
				ifs.close();
			}
        }
        _findclose(handle);
    }
}