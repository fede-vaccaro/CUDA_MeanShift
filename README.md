## CUDA MeanShift ##
*A CUDA implementation of MeanShift clustering algorithm. Written as Parallel Computing course project.*

**depandancies needed:**
* CUDA 10.0
* Boost library

MeanShift is a simple clustering algorithm. It is *embarassingly parallel*, so a natural choice for its implementation is CUDA. Currently it's only for 2D points, but since it's used for Image Segmentation, soon will be added the implementation for RGB image. 

This version is optimized to avoid lots of vram access thanks to the shared memory.

