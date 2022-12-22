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

#include "imgproc.h"

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#else
#include <opencv2/imgproc.hpp>
#endif

#ifdef HAVE_OPENCV_CUDACODEC

void frame_preprocessing(cv::cuda::GpuMat& frame) {
	frame.convertTo(frame, CV_32FC1);
	cv::cuda::divide(frame, 255, frame);
	cv::cuda::resize(frame, frame, cv::Size(128, 128));
	if (frame.channels() == 4) {
		cv::cuda::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
	}
	else if (frame.channels() == 3) {
		cv::cuda::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	}
}

void edge_detection(cv::cuda::GpuMat& frame, cv::Mat& edge_frame) {
	cv::Mat frame_cpu;
	frame.download(frame_cpu);
	edge_detection(frame_cpu, edge_frame);
}

cv::Mat make_noise(const cv::cuda::GpuMat& frame, const double SNR) {
	cv::Mat frame_cpu;
	frame.download(frame_cpu);
	return make_noise(frame_cpu, SNR);
}

#endif

void frame_preprocessing(cv::Mat& frame) {
	frame.convertTo(frame, CV_32FC1);
	cv::divide(frame, 255, frame);
	cv::resize(frame, frame, cv::Size(128, 128));
	if (frame.channels() == 4) {
		cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
	}
	else if (frame.channels() == 3) {
		cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	}
}

void edge_detection(cv::Mat& frame, cv::Mat& edge_frame) {
	cv::GaussianBlur(frame, edge_frame, cv::Size(3, 3), 1, 1, cv::BORDER_DEFAULT);
	cv::Mat sobel_x, sobel_y;
	cv::Mat kernel_x = (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	cv::Mat kernel_y = (cv::Mat_<double>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	cv::filter2D(edge_frame, sobel_x, -1, kernel_x, cv::Point(-1, -1));
	cv::filter2D(edge_frame, sobel_y, -1, kernel_y, cv::Point(-1, -1));
	cv::pow(sobel_x, 2, sobel_x);
	cv::pow(sobel_y, 2, sobel_y);
	cv::sqrt((sobel_x + sobel_y), edge_frame);
}

cv::Mat make_noise(const cv::Mat& frame, const double SNR) {
	cv::Mat noise_frame(frame.size(), frame.type());
	double avg = cv::mean(frame)[0];
	cv::randn(noise_frame, 0, avg / SNR);
	return frame + noise_frame;
}
