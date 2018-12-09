//
// Created by Federico Vaccaro on 30/11/2018.
#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>

#include <sstream>
#include <cmath>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>

#include "csvio.h"



int main() {
    const int nData = 183*324;  // number of experiments

    std::default_random_engine generator;

    std::vector<std::normal_distribution<float>> centroids;

    std::normal_distribution<float> X0(3.0, 0.2);
    std::normal_distribution<float> Y0(3.0, 0.5);

    std::normal_distribution<float> X1(15.0, 1.0);
    std::normal_distribution<float> Y1(17.0, 0.2);

    std::normal_distribution<float> X2(10.5, 1.5);
    std::normal_distribution<float> Y2(0.0, 1.5);

    centroids.push_back(X0);
    centroids.push_back(Y0);
    centroids.push_back(X1);
    centroids.push_back(Y1);
    centroids.push_back(X2);
    centroids.push_back(Y2);

    std::vector<float> points;

    const int nCentroids = centroids.size()/2;

    points.resize(nData * 2);

    for (int i = 0; i < nData; ++i) {
        int centroid = i % nCentroids;
        std::normal_distribution<float> X_i = centroids[centroid*2];
        std::normal_distribution<float> Y_i = centroids[centroid*2 + 1];
        int idx = i * 2;
        points[idx] = X_i(generator);
        points[idx+1] = Y_i(generator);

    }
    std::string delimiter = ";";
    std::string filename = "../dataset.csv";
    write2VecTo(filename.c_str(), delimiter, points);
    //read2VecFrom(filename.c_str(), delimiter, points);

    return 0;
}

