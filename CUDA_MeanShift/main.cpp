#include <iostream>
#include <omp.h>

#include <cuda.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_runtime_api.h"

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>

#include <iostream>
#include <math.h>
#include "csvio.h"

#include <algorithm> // for copy() and assign() 
#include <iterator> // for back_inserter 
#include "vector_functions.hpp"
#include "vector_types.h"
#include "helper_math.h"

#include "commonDefines.h"

extern "C"
void cudaMeanShift_sharedMemory_2D_wrapper(float *X, const float *I, const int N, const int vecDim, dim3 gridDim, dim3 blockDim);
extern "C"
void cudaMeanShift_2D_wrapper(float *X, const float *I, const int N, const int vecDim, dim3 gridDim, dim3 blockDim);

int main() {

	//Loading Data
	std::vector<float> inputData;

	float * InputData_ptr;
	float * OutputData_ptr;


	std::string delimiter = ";";
	std::string filename = "datasets/dataset50000.csv";
	read2VecFrom(filename.c_str(), delimiter, inputData);
	const int vecDim = 2;
	const int datasetDim = inputData.size() / vecDim;


	thrust::device_vector<float> inputData_device = inputData;
	thrust::device_vector<float> outputData_device = inputData;


	float * inputData_ptr_device = thrust::raw_pointer_cast(&inputData_device[0]);
	float * outputData_ptr_device = thrust::raw_pointer_cast(&outputData_device[0]);

	dim3 gridDim = dim3(ceil((float)(datasetDim) / BLOCK_DIM));
	dim3 blockDim = dim3(BLOCK_DIM);

	double t1 = omp_get_wtime();
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		//std::cout << "Iteration n: " << i << " started." << std::endl;
		//cudaMeanShift_sharedMemory_2D_wrapper(outputData_ptr_device, inputData_ptr_device, datasetDim, vecDim, gridDim, blockDim);
		cudaMeanShift_2D_wrapper(outputData_ptr_device, inputData_ptr_device, datasetDim, vecDim, gridDim, blockDim);
		cudaDeviceSynchronize();
		std::swap(inputData_ptr_device, outputData_ptr_device);
	}
	std::swap(inputData_ptr_device, outputData_ptr_device);

	
	double t2 = omp_get_wtime() - t1;
	std::cout << "Mean Shift completed in: " << t2 << std::endl;

	//extracting result and saving
	thrust::host_vector<float> outputData_host = outputData_device;



	std::vector<float> outputData_host_stl;
	outputData_host_stl.resize(outputData_host.size());
	thrust::copy(outputData_host.begin(), outputData_host.end(), outputData_host_stl.begin());

	//find cluster
	t1 = omp_get_wtime();
	std::vector<float2> cluster;


	cluster.push_back(make_float2(outputData_host_stl[0], outputData_host_stl[1]));

	std::vector<float>& points = outputData_host_stl;
	for (int i = 2; i < points.size(); i+=2) {
		bool insideNoCluster = true;
		float2 point = make_float2(points[i], points[i + 1]);
		for (int j = 0; j < cluster.size(); j++) {
			float2 cluster_ = cluster[j];
			float2 sub = cluster_ - point;
			float distance = sqrt(dot(sub, sub));
			if (distance < BW) {
				insideNoCluster = false;
			}
		}
		if (insideNoCluster) {
			cluster.push_back(point);
		}
	}

	for (int i = 0; i < cluster.size(); i++) {
		std::cout << "Cluster " << i << ": " << "(" << cluster[i].x << ", " << cluster[i].y << ")" << std::endl;
	}
	t2 = omp_get_wtime() - t1;
	std::cout << "Time to find clusters: " << t2 << std::endl;

	write2VecTo(std::string("results.csv"), delimiter, outputData_host_stl);

	return 0;
}
