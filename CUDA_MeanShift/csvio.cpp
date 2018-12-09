//
// Created by Federico on 01/12/2018.
//

#include "csvio.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <string>
#include <sstream>


void write2VecTo(std::string filename, std::string delimiter, std::vector<float>& vec){
    std::cout << "Saving to: " + filename << std::endl;

    std::ostringstream valStream;
    std::ofstream myfile(filename);
    std::cout << std::setprecision(7);
    myfile << std::setprecision(7);
    for(int i = 0; i < vec.size(); i+=2){
        valStream << std::fixed << vec[i] << delimiter << vec[i+1] << "\n";
    }
    myfile << valStream.str();
    myfile << std::endl;
    myfile.close();
}

void read2VecFrom(std::string filename, std::string delimiter, std::vector<float>& dest){
    std::cout << "Reading from: " + filename << std::endl;
    std::ifstream file(filename);

    std::string line = ",";
    // Iterate through each line and split the content using delimeter
    while (getline(file, line))
    {
        std::vector<std::string> vec;
        boost::algorithm::split(vec, line, boost::is_any_of(delimiter));
        std::vector<float> xy;
        for(auto i = vec.begin(); i != vec.end(); i++){
            float value = strtof(i->c_str(), NULL);
            if(value)
                dest.push_back( value );
        }
    }

    file.close();
    bool read = false;
    if(read) {
        for (int i = 0; i < dest.size(); i++) {
            std::cout << dest[i] << ", ";
            if ((i % 2))
                std::cout << std::endl;
        }
    }
}
