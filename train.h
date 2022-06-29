#ifndef _TRAIN_H_
#define _TRAIN_H_
#include <cstdlib>
#include "svm.h"
#include "dataset.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))


char model_name[] = "model.h";
svm_parameter param;
class PreprocessSVM{
	public:
		int i, k, j;
		int prob_feature; //dimension of features
		double result;
		void init_parameters();
		int save_parameters(const char *model_file_name);
		void prepare_training_data();
		int prepare_testing_data(svm_model *model, double svm_intput[FEATURES_NUM]);
		void do_cross_validation();
		svm_problem prob;
		svm_node *x_space;
};



#endif /* _TRAIN_H */