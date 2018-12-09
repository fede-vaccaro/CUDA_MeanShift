//
// Created by Federico on 01/12/2018.
//

#ifndef OPENMP_SAMPLES_READFROMCSV_H
#define OPENMP_SAMPLES_READFROMCSV_H

#include <vector>
#include <string>

void write2VecTo(std::string filename, std::string delimiter, std::vector<float>& vec);
void read2VecFrom(std::string filename, std::string delimiter, std::vector<float>& dest);

#endif //OPENMP_SAMPLES_READFROMCSV_H

