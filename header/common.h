/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.

 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <filesystem>
#include <iostream>
#include <mutex>
#include <opencv2/opencv_modules.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/core/cuda.hpp>
#else
#include <opencv2/core/mat.hpp>
#endif

#include "config.h"
#include "debug.h"

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
