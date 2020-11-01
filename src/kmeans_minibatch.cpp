/*
 * main.cxx
 *
 * Copyright 2014 yzhao30 <yzhao30@yzhao30-csc>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Note: ./kmeans_minibatch 3D_spatial_network_shuffled.txt 434874 4 450 450
 */
#include "kmeans_minibatch.h"



void dump(double *x, int n)
{
    for (int i = 0; i < n; i++) {
        cout << setprecision(12) << x[i] << " ";
    }
    cout << endl;
}

double sqrt_distance(double *x, double *y, int data_dim)
{
    int i;
    double diff, dist = 0;
    for (i = 0; i < data_dim; ++i)
    {
        diff = x[i] - y[i];
        dist += diff * diff;
    }
    return sqrt(dist);
}

double distance(double *x, double *y, int data_dim)
{
    int i;
    double diff, dist = 0;
    for (i = 0; i < data_dim; ++i)
    {
        diff = x[i] - y[i];
        dist += diff * diff;
    }
    return dist;
}

MiniBatchKmeans::MiniBatchKmeans(Data *data_obj, int K, int max_iter, int B)
{
    int i;
    int batch_number;
    
    data_num = data_obj->data_num;
    data_dim = data_obj->data_dim;
    if (data_num < K) {
        cout << "data_num is less than K.\n";
        return;
    }
    this->K = K;
    this->max_iter = max_iter;
    batch_size = B;
    batch_number = ceil((double)data_num/(double)B);
    max_iter_on_batch = max_iter * batch_number;
    
    X = data_obj->X;
    
    Centroids = new double*[K]();
    for (i = 0; i < K; ++i) {
        Centroids[i] = new double[data_dim]();
    }
    CentroidsOld = new double*[K]();
    for (i = 0; i < K; ++i) {
        CentroidsOld[i] = new double[data_dim]();
    }
    CenterCount = new int[K]();
    Label = new int[data_num]();
    
    Batch = new int[B]();
    Cache = new int[B]();
    
    srand(time(NULL));
}

MiniBatchKmeans::~MiniBatchKmeans()
{
    for (int i = 0; i < K; ++i) {
        delete [] Centroids[i];
        delete [] CentroidsOld[i];
    }
    delete [] Centroids;
    delete [] CentroidsOld;
    delete [] CenterCount;
    delete [] Label;
    delete [] Batch;
    delete [] Cache;
}

void MiniBatchKmeans::initCentroids()
{
    int i, j;
    debug(DEBUGMSG, "initialize centroids ...");
    /*
    double *tmp;
    for (i = 0; i < K; ++i) {
        j = rand() % (data_num - i);
        if (j != 0) {
            // swap X[i] and X[i + j]
            tmp = X[i];
            X[i] = X[i + j];
            X[i + j] = tmp;
        }
    }
     */
    for (i = 0; i < K; ++i) {
        for (j = 0; j < data_dim; j++) {
            Centroids[i][j] = X[i][j];
        }
    }
    /*
     for (i = 0; i < K; ++i) {
     dump(Centroids[i], data_dim);
     }
     */
}

void MiniBatchKmeans::fit(int max_no_improvement)
{
    int i, j, t, b, c;
    double eta, eta_min = -1;
    int no_improvement = 0;
    
    debug(DEBUGMSG, "begin iteration...");
    for (t = 0; t < max_iter_on_batch; ++t)
    {
        /* pick batch_size examples from X randomly */
        for (b = 0; b < batch_size; ++b) {
            Batch[b] = rand() % data_num;
        }
        // Cache the center nearest to x
        for (b = 0; b < batch_size; ++b) {
            Cache[b] = getNearestCenter(X[Batch[b]]);
        }
        // Copy the centers
        for (i = 0; i < K; i++) {
            for (j = 0; j < data_dim; j++) {
                CentroidsOld[i][j] = Centroids[i][j];
            }
        }
        // Update the centers
        for (b = 0; b < batch_size; ++b) {
            c = Cache[b];
            CenterCount[c]++;
            eta = 1.0 / CenterCount[c];
            double *xb = X[Batch[b]];
            
            for (i = 0; i < data_dim; ++i) {
                Centroids[c][i] = (1.0 - eta) * Centroids[c][i] + eta * xb[i];
            }
        }
        
        // Optional, monitor convergence and early stop
        //Control early stopping based on the consecutive number of mini 
        //batches that does not yield an improvement on the smoothed inertia.        
        //The value of the inertia criterion associated with the chosen 
        //partition (if compute_labels is set to True). The inertia is 
        //defined as the sum of square distances of samples to their 
        //nearest neighbor.
        eta = 0;
        for (b = 0; b < batch_size; ++b) {
            c = getNearestCenter(X[Batch[b]]);
            eta += distance(Centroids[c], X[Batch[b]], data_dim);
        }
        if (eta_min == -1 || eta < eta_min) {
            no_improvement = 0;
            eta_min = eta;
        }
        else {
            no_improvement++;
        }

        if (no_improvement == max_no_improvement) {
            break;
        }
    }
    labelDataWithCenter();
    
    std::cout << "K, batch_size, error, iter_num\n";
    cout << K << ", " << batch_size << ", " << calculateError() << ", " << t << endl;
}

/* Private functions */
int MiniBatchKmeans::getNearestCenter(double *x)
{
    int i, c;
    double dist, dist_min;
    dist_min = distance(Centroids[0], x, data_dim);
    c = 0;
    for (i = 1; i < K; ++i)
    {
        dist = distance(Centroids[i], x, data_dim);
        if (dist < dist_min) {
            dist_min = dist;
            c = i;
        }
    }
    return c;
}

bool MiniBatchKmeans::labelDataWithCenter()
{
    bool changed = false;
    int cbest;
    for (int i = 0; i < data_num; i++) {
        cbest = getNearestCenter(X[i]);
        //cout << Label[i] << endl;
        if (cbest != Label[i]) {
            Label[i] = cbest;
            changed = true;
        }
    }
    return changed;
}

double MiniBatchKmeans::calculateError()
{
    double dist = 0;
    for (int i = 0; i < data_num; i++) {
        dist += sqrt_distance(X[i], Centroids[Label[i]], data_dim);
    }
    return dist;
}


