#ifndef MEANSHIFT_CU
#define MEANSHIFT_CU

#include <cuda.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_runtime_api.h"

#include "vector_functions.hpp"
#include "vector_types.h"
#include "helper_math.h"
#include "device_functions.h" 
#include "commonDefines.h"


#define MYASSERT(condition, ERROR) if (!(condition)) { printf("ERROR: %s \n", ERROR); return; }
#define rev_sqrt_two_pi 0.3989422804
#define rev_two_pi rev_sqrt_two_pi*rev_sqrt_two_pi

__device__ __host__ float gaussian_kernel(float dist2, float bandwidth) {
	const float rev_bandwidth = 1. / bandwidth;
	const float d2_frac_b2 = dist2 * rev_bandwidth * rev_bandwidth;
	float div = 1. / rev_two_pi * rev_bandwidth;
	float exp_ = div * expf(- 0.5 * d2_frac_b2);
	return exp_;
}

__global__ void cuda_MeanShift_SharedMemory_2D(float *X, const float *I, const float * originalPoints, const int N, const int dim) {

	__shared__ float tile[TILE_WIDTH][2];

	// for each pixel
	int tx = threadIdx.x;
	int row = blockIdx.x*blockDim.x + tx;

	float2 numerator = make_float2(0.0, 0.0);
	float denominator = 0.0;
	int it = row * dim;

	for (int tile_i = 0; tile_i < (N - 1) / TILE_WIDTH + 1; ++tile_i) {
		//loading phase - each thread load something into shared memory
		int row_t = tile_i * TILE_WIDTH + tx;

		int index = row_t * dim;
		if (row_t < N) {
			tile[tx][0] = originalPoints[index];
			tile[tx][1] = originalPoints[index + 1];
		}
		else {
			tile[tx][0] = 0.0;
			tile[tx][1] = 0.0;
		}
		__syncthreads();
		//end of loading into shared memory

		if (row < N) // only the threads inside the bounds do some computation
		{
			float2 x_i = make_float2(I[it], I[it + 1]); //load input point

			//computing phase
			for (int j = 0; j < TILE_WIDTH; ++j) {
				float2 x_j = make_float2(tile[j][0], tile[j][1]); //from shared memory
				float2 sub = x_i - x_j;
				float distance2 = dot(sub, sub);
				float weight = gaussian_kernel(distance2, BW);
				numerator += x_j * weight; //accumulating
				denominator += weight;

			}
		}
		__syncthreads();
		//end of computing phase for tile_ij
	}

	if (row < N) {
		//storing
		numerator /= denominator;
		X[it] = numerator.x;
		X[it + 1] = numerator.y;
	}

}

extern "C"
void cudaMeanShift_sharedMemory_2D_wrapper(float *X, const float *I, const float * originalPoints, const int N, const int vecDim, dim3 gridDim, dim3 blockDim) {
	cuda_MeanShift_SharedMemory_2D <<<gridDim, blockDim >>> (X, I, originalPoints, N, vecDim);
}

__global__ void cuda_MeanShift_2D(float *X, const float *I, const float * originalPoints, const int N, const int dim) {

	// for every pixel
	int tx = threadIdx.x;
	int row = blockIdx.x*blockDim.x + tx;

	float2 numerator = make_float2(0.0, 0.0);
	float denominator = 0.0;

	int it = row * dim;
	float2 y_i;
	if (row < N) {
		y_i = make_float2(I[it], I[it + 1]); //load input point

			//computing mean shift
			for (int j = 0; j < N; ++j) {
				float2 x_j = make_float2(originalPoints[j*dim], originalPoints[j*dim + 1]); //from central gpu memory
				float2 sub = y_i - x_j;
				float distance2 = dot(sub, sub);
				float weight = gaussian_kernel(distance2, BW);
				numerator += x_j * weight; //accumulating
				denominator += weight;
			}

		//storing
		numerator /= denominator;
		X[it] = numerator.x;
		X[it + 1] = numerator.y;
	}

}

extern "C"
void cudaMeanShift_2D_wrapper(float *X, const float *I, const float * originalPoints, const int N, const int vecDim, dim3 gridDim, dim3 blockDim) {
	cuda_MeanShift_2D <<<gridDim, blockDim >>> (X, I, originalPoints, N, vecDim);
}

#endif // !MEANSHIFT_CU
