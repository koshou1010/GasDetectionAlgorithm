// #include "mbed.h"
#include <iostream>
#include <string.h>
#include <math.h>
#include "train.h"
#include "dataset.h"
#include "svm.h"
#include "model.h"
#include "utils.h"
#include "classifier.h"
#include "algo.h"
#include "mbed.h"
using namespace std;
// extern Serial SYS_Console;

double predict_result;
extern ReturnInfo return_info;

// extern DigitalOut Debugpin1;

/*****************************svm part start**********************************/

/*load svm trained parameters*/
void load_parameters(svm_model *model, svm_model_trained loadparam)
{
        svm_parameter &param = model->param;
        model->param.svm_type = loadparam.param.svm_type;
        model->param.kernel_type = loadparam.param.kernel_type;
        model->param.degree = loadparam.param.degree;
        model->param.gamma = loadparam.param.gamma;
        model->param.coef0 = loadparam.param.coef0;
        model->param.cache_size = loadparam.param.cache_size;
        model->param.eps = loadparam.param.eps;
        model->param.C = loadparam.param.C;

        model->param.nr_weight = 0;
        model->param.weight_label = NULL;
        model->param.weight = NULL;
        model->param.nu = loadparam.param.nu;
        model->param.p = loadparam.param.p;
        model->param.shrinking = loadparam.param.shrinking;
        model->param.probability = loadparam.param.probability;

        model->l = loadparam.l;
        model->nr_class = loadparam.nr_class;
        int a = model->nr_class * (model->nr_class - 1) / 2;
        model->rho = Malloc(double, a);
        for (int i = 0; i < a; i++)
        {
                model->rho[i] = loadparam.rho[i];
        }
        int b = model->nr_class;
        model->label = Malloc(int, b);
        for (int i = 0; i < b; i++)
                model->label[i] = loadparam.label[i];
        int c = model->nr_class;
        model->nSV = Malloc(int, c);
        for (int i = 0; i < c; i++)
                model->nSV[i] = loadparam.nSV[i];
}

/*load svm trained model*/
svm_model *load_model(ClassifierSVM csvm)
{
	svm_model_trained loadparam;
	svm_model *model_trained = Malloc(svm_model,1);
	model_trained->rho = NULL;
	model_trained->probA = NULL;
	model_trained->probB = NULL;
	model_trained->sv_indices = NULL;
	model_trained->label = NULL;
	model_trained->nSV = NULL;
	load_parameters(model_trained, loadparam);

	/*process sv_coef and SV*/	
	int elements = 0;
	int max_line_len = 1024;
	char *line = Malloc(char,max_line_len);
	char *p,*endptr,*idx,*val;
	for(int i = 0; i < model_trained->l; i++)
        {
		p = strtok(loadparam.all_sv_split[i],":");
		while(1)
		{
			p = strtok(NULL,":");
			if(p == NULL)
				break;
			elements++;
		}
	}
	elements += model_trained->l;
	int m = model_trained->nr_class - 1;
	int l = model_trained->l;
	model_trained->sv_coef = Malloc(double *,m);
	int i;
	for(i=0;i<m;i++)
		model_trained->sv_coef[i] = Malloc(double,l);
	model_trained->SV = Malloc(svm_node*,l);
	svm_node *x_space = NULL;
	if(l>0) x_space = Malloc(svm_node,elements);
	int j=0;
	for(i=0;i<l;i++)
	{
		model_trained->SV[i] = &x_space[j];
		p = strtok(loadparam.all_sv[i], " \t");
		model_trained->sv_coef[0][i] = strtod(p,&endptr);
		for(int k=1;k<m;k++)
		{
			p = strtok(NULL, " \t");
			model_trained->sv_coef[k][i] = strtod(p,&endptr);
		}
		while(1)
		{
			idx = strtok(NULL, ":");
			val = strtok(NULL, " \t");
			if(val == NULL)
				break;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			x_space[j].value = strtod(val,&endptr);
			++j;
		}
		x_space[j++].index = -1;
	}
	free(line);
	model_trained->free_sv = 1;	// XXX
	return model_trained;	
}

/*assign_testing_data and predict result*/
double ClassifierSVM::assign_testing_data(svm_model *model, double *svm_intput)
{
        int k,j;
        double result;
        // assign testing data
        svm_node xnode[(ONE_FEATURE + 1)];
        for (k = 0; k < (ONE_FEATURE + 1); k++)
        {
                for (j = 0; j < ONE_FEATURE; j++)
                {
                        if (k % (ONE_FEATURE + 1) == j)
                        {
                                xnode[k].index = j+1;
                                xnode[k].value = svm_intput[j];
                        }
                }
                if (k % (ONE_FEATURE + 1) == ONE_FEATURE)
                {
                        xnode[k].index = -1;
                }
        }

        result = svm_predict(model, xnode);
        return result;
}

/*svm run including assign testdata and predict*/
void ClassifierSVM::svm_run(svm_model *modelh)
{
        double svm_intput[ONE_FEATURE] = {0};
        /* fake svm input
        for (int i = 0; i < 42; i++)
                svm_intput[i] = test_dataset[0][i+1]; 
        */
        for (int i = 0; i < ONE_FEATURE; i++)
        {
                svm_intput[i] = return_info.feature_set[rmdr][i];
        }
        predict_result = assign_testing_data(modelh, svm_intput);
        return_info.predict_label = (int)predict_result;
        if (return_info.predict_label == 0)
        {
                return_info.predict_labelname = "DMMP";
        }
        else if (return_info.predict_label == 1)
        {
                return_info.predict_labelname = "MS";
        }
        else if (return_info.predict_label == 2)
        {
                return_info.predict_labelname = "TIC";
        }
        myprintf("classifier,svm,%s\r\n", return_info.predict_labelname);
                 
}
/*****************************svm part end************************/

/****************************knn part start***********************/
struct DataVector
{
    int label;
    char labelname[6];
    float value[sensor_num];
};
struct DistanceStruct
{
    int label;
    float distance;
    char labelname[6];
};
struct DataVector training_set[DATASET_NUM];
struct DataVector input_data;
struct DistanceStruct k_nearest_distance[K]; //K-Nearest distance

/*-----calculate Euclidean distance-----*/
float ClassifierKNN::distance(struct DataVector vector1, struct DataVector vector2)
{
    float dist = 0.0;
    float sum = 0.0;
    for (int i = 0; i < sensor_num; i++)
    {
        sum += (vector1.value[i] - vector2.value[i]) * (vector1.value[i] - vector2.value[i]);
    }
    dist = sqrt(sum);
    return dist;
}
/*-----find the maximum distance in k_nearest_distance-----*/
int ClassifierKNN::get_max_distance()
{
    int maxNo = 0;
    for (int i = 1; i < K; i++)
    {
        if (k_nearest_distance[i].distance > k_nearest_distance[maxNo].distance)
            maxNo = i;
    }
    return maxNo;
}

char *ClassifierKNN::Classify(struct DataVector input_data)
{
        float dist = 0;
        int maxlabel = 0;
        int freq[K];
        int tmpfreq = 1;
        char *PredictLabel = k_nearest_distance[0].labelname;
        int predictlabel = 0;
        memset(freq, 1, sizeof(freq)); // init all elements of freq as 1

        /*-----step1: init distance as max value-----*/
        for (int i = 0; i < K; i++)
        {
                k_nearest_distance[i].distance = MAX_VALUE;
        }
        /*-----step2: calculate K - nearest distance-----*/
        for (int i = 0; i < DATASET_NUM; i++)
        {
                //step.2.1---calculate distance of input data and each train_dataset data
                dist = distance(training_set[i], input_data);
                //step.2.2---got max distance of k_nearest_distance
                maxlabel = get_max_distance();
                //step.2.3---if this distance < got max distance of k_nearest_distance, as K-nearest
                if (dist < k_nearest_distance[maxlabel].distance)
                {
                        k_nearest_distance[maxlabel].label = training_set[i].label;
                        k_nearest_distance[maxlabel].distance = dist;
                        strcpy(k_nearest_distance[maxlabel].labelname, training_set[i].labelname);
                }
        }
        // printf("The most nearest distance are : ");
        for (int i = 0; i < K; i++)
        {
        // printf("%s,", k_nearest_distance[i].labelname);
        }
        // printf("\r\n");
        /*-----step3: count how many times at each class-----*/
        for (int i = 0; i < K; i++)
        {
                for (int j = 0; j < K; j++)
                {
                        if ((i != j) && (strcmp(k_nearest_distance[i].labelname, k_nearest_distance[j].labelname) == 0))
                        {
                        freq[i] += 1;
                        }
                }
        }
        /*-----step4: choose the maximum class as predict label-----*/
        for (int i = 0; i < K; i++)
        {
                if (freq[i] > tmpfreq)
                {
                        tmpfreq = freq[i];
                        PredictLabel = k_nearest_distance[i].labelname;
                        predictlabel = k_nearest_distance[i].label;
                }
        }
    // strcpy(Return_info.predict_name, PredictLabel);
    // Return_info.predict_label = predictlabel;
        return_info.predict_label = predictlabel;
        return_info.predict_labelname = PredictLabel;
        return PredictLabel;
}



void ClassifierKNN::knn()
{
        const char *classLabel;
        for (int i = 0; i < sensor_num; i++)
        {
                input_data.value[i] = return_info.feature_set[rmdr][i];
        }
        /* set train_dataset and Label to training_set struct */
        for (int i = 0; i < DATASET_NUM; i++)
        {
                for (int k = 0; k < sensor_num; k++)
                {
                        training_set[i].value[k] = train_dataset[i][k+1];
                        training_set[i].label = train_dataset[i][0];
                }
                if (training_set[i].label == 0)
                {
                        strcpy(training_set[i].labelname, "DMMP");
                }
                if (training_set[i].label == 1)
                {
                        strcpy(training_set[i].labelname, "MS");
                }
                if (training_set[i].label == 2)
                {
                        strcpy(training_set[i].labelname, "TIC");
                }
        }
        classLabel = Classify(input_data);
        // printf("classifier,knn,%i\r\n",return_info.knn_predict_label);
}

/*****************************knn part end*****************************/
