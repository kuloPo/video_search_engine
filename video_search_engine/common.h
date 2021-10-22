#pragma once

#include <filesystem>
#include <iostream>
#include <opencv2/core/cuda.hpp>

struct Key_Frame {
	double delta;
	cv::cuda::GpuMat first_frame;
	cv::cuda::GpuMat second_frame;
	int frame_num;
};

inline const std::filesystem::path video_path = ".\\videos";
inline const std::filesystem::path csv_path = ".\\data\\interval.csv";
inline const std::filesystem::path key_frame_path = ".\\data";