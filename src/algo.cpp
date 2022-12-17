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

#include "algo.h"

#include <algorithm>
#include <numeric>
#include <iomanip>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#else
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#endif

#include "similar.h"
#include "utils.h"
#include "io.h"
#include "imgproc.h"

Keyframe_Detector::Keyframe_Detector(const std::filesystem::path& filename) {
	this->filename = filename;
	this->total_frames = get_total_frames(filename);
}

std::vector<Key_Frame*> Keyframe_Detector::run() {
	cv::TickMeter frame_time;

	this->init_video_reader();

	// entire video is emtpy
	if (first_frame.empty()) {
		add_key_frame(key_frames, 0, 0, empty_frame, empty_frame);
		return key_frames;
	}

	this->frame_process(first_frame, first_radon);

	// add first frame into index
	add_key_frame(key_frames, 0, 0, first_frame, empty_frame);

	// variables for measuring performance
	frame_count = 0;

	while (true) {
		frame_time.reset(); frame_time.start();
		if (!this->read_frame())
			break;

		if (frame_count % (jumped_frame + 1) == 0) {
			this->frame_process(second_frame, second_radon);

			double d = radon_distance(first_radon, second_radon);

			// If delta is greater threshold, write the information into vector
			if (d > frame_difference_threshold) {
				add_key_frame(key_frames, d, frame_count, first_frame, second_frame);
			}

			first_frame = second_frame;
			first_radon = second_radon;
		}

		frame_time.stop();
		times.push_back(frame_time.getTimeMilli());
		frame_count++;
	}

	// add last frame into index
	add_key_frame(key_frames, 0, frame_count, first_frame, empty_frame);

	return key_frames;
}

void Keyframe_Detector::init_video_reader() {
#ifdef HAVE_OPENCV_CUDACODEC
	cuda_reader = cv::cudacodec::createVideoReader(filename.string());
	cuda_reader->nextFrame(first_frame);
#else
	video_reader = cv::VideoCapture(filename.string());
	video_reader >> first_frame;
#endif
}

bool Keyframe_Detector::read_frame() {
#ifdef HAVE_OPENCV_CUDACODEC
	return cuda_reader->nextFrame(second_frame);
#else 
	video_reader >> second_frame;
	return !second_frame.empty();
#endif
}

#ifdef HAVE_OPENCV_CUDACODEC
void Keyframe_Detector::frame_process(cv::cuda::GpuMat& in_frame, cv::Mat& out_frame) {
#else
void Keyframe_Detector::frame_process(cv::Mat & in_frame, cv::Mat & out_frame) {
#endif
	// frame preprocessing
	frame_preprocessing(in_frame);
	// edge detection
	edge_detection(in_frame, edge_frame);
	// calculate histogram and the distance between hist
	Radon_Transform(edge_frame, out_frame, 45, 0, 180);
}

void Keyframe_Detector::print_performance() {
	if (!times.empty()) {
		std::sort(times.begin(), times.end());
		double total_time = std::accumulate(times.begin(), times.end(), 0.0);
		double avg = total_time / frame_count;
		safe_printf("%s %.2f %d %.2f\n", filename.filename().string().c_str(), total_time, frame_count, avg);
	}
}

cv::Rect find_bounding_box(const std::filesystem::path& video_path) {
	cv::Mat frame;
	int gpu_frame_count = 0;
	cv::Rect bounding_box, tmp_box;
	std::vector<int> x, y, w, h;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat gpu_frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(video_path.string());
	cuda_reader->nextFrame(gpu_frame);
	gpu_frame.download(frame);
#else
	cv::VideoCapture video_reader(video_path.string());
	video_reader >> frame;
#endif
	cv::Size raw_size = frame.size();
	while (true) {
#ifdef HAVE_OPENCV_CUDACODEC
		if (!cuda_reader->nextFrame(gpu_frame))
			break;
		cv::cuda::cvtColor(gpu_frame, gpu_frame, cv::COLOR_BGRA2GRAY);
		gpu_frame.download(frame);
#else 
		video_reader >> frame;
		if (frame.empty())
			break;
		cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
#endif
		cv::threshold(frame, frame, 30, 255, cv::THRESH_BINARY);
		tmp_box = cv::boundingRect(frame);
		x.push_back(tmp_box.x);
		y.push_back(tmp_box.y);
		w.push_back(tmp_box.width);
		h.push_back(tmp_box.height);
		gpu_frame_count++;
	}
	int p_x = vector_median(x);
	int p_y = vector_median(y);
	int width = std::min(vector_median(w), raw_size.width - p_x);
	int height = std::min(vector_median(h), raw_size.height - p_y);
	return cv::Rect(p_x, p_y, width, height);
}
