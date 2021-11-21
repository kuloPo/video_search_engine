#pragma once

#include <filesystem>
#include <iostream>
#include <opencv2/core/cuda.hpp>

struct Key_Frame {
	double delta;
	int frame_num;
	cv::cuda::GpuMat first_frame;
	cv::cuda::GpuMat second_frame;
};

inline const std::filesystem::path video_path = "..\\videos";
inline const std::filesystem::path index_path = "..\\data";

