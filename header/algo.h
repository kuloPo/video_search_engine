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

#include "io.h"
#include "common.h"

//class Keyframe_Detector {
//public:
//	Keyframe_Detector(const std::filesystem::path& filename);
//	std::vector<Key_Frame*> run();
//	void print_performance();
//protected:
//	void init_video_reader();
//	virtual bool read_frame();
//	virtual void frame_process(cv::Mat& in_frame, cv::Mat& out_frame);
//
//protected:
//	std::filesystem::path filename;
//	std::vector<Key_Frame*> key_frames;
//
//	int frame_count;
//	std::vector<double> times;
//
//	cv::Mat frame;
//	cv::Mat last_frame;
//
//	cv::Mat first_radon;
//	cv::Mat second_radon;
//	cv::Mat edge_frame;
//
//#ifdef HAVE_OPENCV_CUDACODEC
//	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader;
//	cv::cuda::GpuMat frame_gpu;
//#else
//	cv::VideoCapture video_reader;
//#endif
//};

class Keyframe_Detector : public Video_Reader {
public:
	Keyframe_Detector(const std::filesystem::path& filename);
	std::vector<Key_Frame*> get_index();
protected:
	virtual bool preprocess();
	virtual void frame_operation();
	virtual void postprocess();
	virtual void frame_process(AutoMat& in_frame, cv::Mat& out_frame);

protected:
	AutoMat last_frame;
	cv::Mat edge_frame;
	cv::Mat radon;
	cv::Mat last_radon;
	std::vector<Key_Frame*> key_frames;
};

cv::Rect find_bounding_box(const std::filesystem::path& video_path);
