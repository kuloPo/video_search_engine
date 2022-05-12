/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <opencv2/opencv_modules.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#include <opencv2/cudawarping.hpp>
#else
#include <opencv2/videoio.hpp>
#endif

#include "common.h"
#include "similar.h"

/*
@brief This function calculates the position of key frames of the given video
by comparing delta between frames.
The return result will contain the frame number, delta of the pairs, 
and reduced size frame images of the pairs. 
First and last frame of a video will always be returned.

@param filename Path of the video to create index
@return A vector containing the pointers of Key_Frame
*/
std::vector<Key_Frame*> create_index(const std::filesystem::path& filename);

/*
@brief This function calculates the interval (in frame number) of a video
given its vector of Key_Frame.

@param key_frames Vector containing the pointers of Key_Frame of the source video
@param interval Destination of the interval vector
*/
void calc_interval(const std::vector<Key_Frame*>& key_frames, std::vector<int>& interval);

#ifdef HAVE_OPENCV_CUDACODEC

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num, 
	cv::cuda::GpuMat first_frame, cv::cuda::GpuMat second_frame);

void frame_preprocessing(cv::cuda::GpuMat& frame);

void edge_detection(cv::cuda::GpuMat& frame, cv::Mat& edge_frame);

#else

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::Mat first_frame, cv::Mat second_frame);

void frame_preprocessing(cv::Mat& frame);

void edge_detection(cv::Mat& frame, cv::Mat& edge_frame);

#endif