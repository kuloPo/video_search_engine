#pragma once

#include <filesystem>
#include <iostream>
#include <opencv2/core/cuda.hpp>
#include "config.h"

using std::cout;
using std::endl;

struct Key_Frame {
	double delta;
	int frame_num;
	cv::cuda::GpuMat first_frame;
	cv::cuda::GpuMat second_frame;
};
