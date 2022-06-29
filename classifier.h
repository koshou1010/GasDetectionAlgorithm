#ifndef _CLASSIFIER_H_
#define _CLASSIFIER_H_


#include "svm.h"
/*class of svm*/
class ClassifierSVM{
    public:
        int i,k,j;
        int svm_model_flag = 0;
        double result;
        double assign_testing_data(svm_model *model, double *svm_intput);
        void svm_run(svm_model *modelh);
};
struct svm_model *load_model(ClassifierSVM csvm);



#define MAX_VALUE 10000.0
#define K 7
class ClassifierKNN{
    public:
        void knn();
        char *Classify(struct DataVector InputData);
        float distance(struct DataVector vector1, struct DataVector vector2);
        int get_max_distance();
};



#endif /* _CLASSIFIER_H_ */