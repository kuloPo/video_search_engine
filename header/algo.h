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
#include <opencv2/opencv_modules.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#else
#include <opencv2/videoio.hpp>
#endif

#include "common.h"

enum MODE {
	INDEXER,
	SEARCHER,
};

class Keyframe_Detector {
public:
	Keyframe_Detector(const std::filesystem::path& filename);
	std::vector<Key_Frame*> run();
private:
	void filter(cv::Mat& frame);
	void init_video_reader();
	bool read_frame();
#ifdef HAVE_OPENCV_CUDACODEC
	void frame_process(cv::cuda::GpuMat& in_frame, cv::Mat& out_frame);
#else
	void frame_process(cv::Mat& in_frame, cv::Mat& out_frame);
#endif

private:
	std::filesystem::path filename;
	std::vector<Key_Frame*> key_frames;

	int total_frames;
	int frame_count;

	cv::Mat first_radon;
	cv::Mat second_radon;
	cv::Mat edge_frame;

#ifdef HAVE_OPENCV_CUDACODEC
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader;
	cv::cuda::GpuMat first_frame;
	cv::cuda::GpuMat second_frame;
#else
	cv::VideoCapture video_reader;
	cv::Mat first_frame;
	cv::Mat second_frame;
#endif
};

/*
@brief This function calculates the position of key frames of the given video
by comparing delta between frames.
The return result will contain the frame number, delta of the pairs, 
and reduced size frame images of the pairs. 
First and last frame of a video will always be returned.

@param filename Path of the video to create index
@return A vector containing the pointers of Key_Frame
*/
std::vector<Key_Frame*> create_index(const std::filesystem::path& filename, const MODE mode = MODE::INDEXER, double video_length_cut = 1.0);

/*
@brief This function calculates the interval (in frame number) of a video
given its vector of Key_Frame.

@param key_frames Vector containing the pointers of Key_Frame of the source video
@param interval Destination of the interval vector
*/
void calc_interval(const std::vector<Key_Frame*>& key_frames, std::vector<int>& interval);

cv::Rect find_bounding_box(const std::filesystem::path& video_path);

#ifdef HAVE_OPENCV_CUDACODEC

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num, 
	cv::cuda::GpuMat first_frame, cv::cuda::GpuMat second_frame);

void frame_preprocessing(cv::cuda::GpuMat& frame);

void edge_detection(cv::cuda::GpuMat& frame, cv::Mat& edge_frame);

#else

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::Mat first_frame, cv::Mat second_frame);

void frame_preprocessing(cv::Mat& frame);

#endif

void edge_detection(cv::Mat& frame, cv::Mat& edge_frame);