#ifndef MINIBATCHKMEANS_H
#define MINIBATCHKMEANS_H
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>

#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"

#define DEBUGMSG false
#define MAX_ITER 100
#define BATCH_SIZE 16
#define MAX_NO_IMPROVEMENT 50
using namespace std;

void debug(bool on, string msg)
{
    if (on) {
        cout << msg << endl;
    }
}
struct Data {
    int data_num; 
    int data_dim; 
    double **X;
    Data(int N, int F) {
        data_num = N;
        data_dim = F;
        X = new double*[data_num];
        for (int i = 0; i < data_num; ++i) {
            X[i] = new double[data_dim]();
        }
    }
    ~Data() {
        for (int i = 0; i < data_num; ++i) {
            delete [] X[i];
        }
        delete [] X;
        debug(DEBUGMSG, "data destruct!");
    }
    int loadData(const std::vector<cv::Point> listOfPoints) {
        for (int i = 0; i < data_num; ++i) {
            cv::Point tmp=listOfPoints[i];
            for (int j = 0; j < data_dim; ++j) {
                X[i][j] = tmp.x;
                X[i][j] = tmp.y;
            }
        }
        return 0;
    }
};

class MiniBatchKmeans {
public:
    MiniBatchKmeans(Data *data_obj, int K, int max_iter, int batch_size);
    
    void initCentroids();
    cv::Point fit(int max_no_improvement);
    bool labelDataWithCenter();
    double calculateError();
    
    void check() {
        cout << "data_num\t data_dim\t K\t max_iter\t batch_size\t max_iter_on_batch\t\n";
        cout << data_num << "\t" << data_dim << "\t" \
        << K << "\t" << max_iter << "\t" << batch_size << "\t" << max_iter_on_batch << endl;
    }
    
    ~MiniBatchKmeans();
    
private:
    /* Data set */
    int data_num;
    int data_dim;
    
    /* Kmeans */
    int K;
    int max_iter; // Iterations over the entire samples
    int batch_size;
    int max_iter_on_batch; // Iterations over batches
    double **X; // A pointer copy to the actual data, No allocation
    double **Centroids; // [K]
    double **CentroidsOld;
    int *CenterCount; // Number of data of each center
    int *Label; // Center label for each data, [data_num]
    
    int *Batch; // Indices for batch samples, [batch_size]
    int *Cache; // Cache of the centroids, [batch_size]
    
    int getNearestCenter(double *x);
    
};
#endif
