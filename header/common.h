#pragma once

#include <filesystem>
#include <iostream>
#include <opencv2/core/cuda.hpp>
#include "config.h"

using std::cout;
using std::endl;

struct Key_Frame {
	double delta;                  // delta value returned by comparison algorithm
	int frame_num;                 // frame number in source video 
	cv::cuda::GpuMat first_frame;  // image of the first frame of the pair
	cv::cuda::GpuMat second_frame; // image of the second frame of the pair
};
