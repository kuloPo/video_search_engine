#pragma once

#include <filesystem>
#include <iostream>
#include <opencv2/opencv_modules.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/core/cuda.hpp>
#else
#include <opencv2/core/mat.hpp>
#endif

#include "config.h"

using std::cout;
using std::endl;

#ifdef HAVE_OPENCV_CUDACODEC
struct Key_Frame {
	double delta;                  // delta value returned by comparison algorithm
	int frame_num;                 // frame number in source video 
	cv::cuda::GpuMat first_frame;  // image of the first frame of the pair
	cv::cuda::GpuMat second_frame; // image of the second frame of the pair
};
inline cv::cuda::GpuMat empty_frame;
#else
struct Key_Frame {
	double delta;                  // delta value returned by comparison algorithm
	int frame_num;                 // frame number in source video 
	cv::Mat first_frame;           // image of the first frame of the pair
	cv::Mat second_frame;          // image of the second frame of the pair
};
inline cv::Mat empty_frame;
#endif