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
#else
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#endif

#include "similar.h"
#include "utils.h"
#include "io.h"

std::vector<Key_Frame*> create_index(const std::filesystem::path& filename, const MODE mode, double video_length_cut) {
	int total_frame = get_total_frames(filename);
	cv::TickMeter index_time;
	index_time.reset(); index_time.start();
	cv::Mat first_radon, second_radon, edge_frame, edge_frame_norm ,edge_frame_prev;
	std::vector<Key_Frame*> key_frames;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat first_frame, second_frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filename.string());
	cuda_reader->nextFrame(first_frame);
#else
	cv::Mat first_frame, second_frame;
	cv::VideoCapture video_reader(filename.string());
	video_reader >> first_frame;
#endif
	if (first_frame.empty()) {
		add_key_frame(key_frames, 0, 0, empty_frame, empty_frame);
		return key_frames;
	}

	cv::Rect bounding_box = mode == MODE::SEARCHER ? find_bounding_box(filename) : cv::Rect();
	if (bounding_box != cv::Rect()) {
		first_frame = first_frame(bounding_box);
	}
	frame_preprocessing(first_frame);
	edge_detection(first_frame, edge_frame);

	edge_frame.convertTo(edge_frame, CV_32FC1);
	edge_frame_norm = edge_frame / sum(edge_frame);

	//first_frame.download(first_frame_cpu);
	Radon_Transform(edge_frame_norm, first_radon, 45, 0, 180);
	
	// add first frame into index
	add_key_frame(key_frames, 0, 0, first_frame, empty_frame);
	
	// variables for measuring performance
	cv::TickMeter tm;
	std::vector<double> times;
	int frame_count = 0;
	
	while (true) {
		tm.reset(); tm.start();
#ifdef HAVE_OPENCV_CUDACODEC
		if (!cuda_reader->nextFrame(second_frame))
			break;
#else 
		video_reader >> second_frame;
		if (second_frame.empty())
			break;
#endif
		if (frame_count > total_frame * video_length_cut) {
			break;
		}
#ifdef SHOW_PROGRESS
		if (gpu_frame_count % 1000 == 0) {
			cout << gpu_frame_count << " frames completed" << endl;
		}
#endif
		if (frame_count % (jumped_frame + 1) == 0) {
			if (bounding_box != cv::Rect()) {
				second_frame = second_frame(bounding_box);
			}
			// frame preprocessing
			frame_preprocessing(second_frame);
			edge_detection(second_frame, edge_frame);

			edge_frame.convertTo(edge_frame, CV_32FC1);
			edge_frame_norm = edge_frame / sum(edge_frame);

			// calculate histogram and the distance between hist
			Radon_Transform(edge_frame_norm, second_radon, 45, 0, 180);

			double d = radon_distance(first_radon, second_radon);

			// If delta is greater threshold, write the information into vector
			if (d > frame_difference_threshold) {
				add_key_frame(key_frames, d, frame_count, first_frame, second_frame);
			}

			first_frame = second_frame;
			second_radon.copyTo(first_radon);
		}

		tm.stop();
		times.push_back(tm.getTimeMilli());
		frame_count++;
	}
	
	// add last frame into index
	add_key_frame(key_frames, 0, frame_count, first_frame, empty_frame);
	index_time.stop();
	
#if defined DEBUG_CREATE_INDEX || defined DEBUG_PERFORMANCE
	if (!times.empty())	{
		std::sort(times.begin(), times.end());
		double total_time = std::accumulate(times.begin(), times.end(), 0.0);
		double avg = total_time / frame_count;
		printf("%s %.2f %d %.2f\n", filename.filename().string().c_str(), total_time, frame_count, index_time.getTimeSec());
	}
#endif // DEBUG_CREATE_INDEX
	
	return key_frames;
}

void calc_interval(const std::vector<Key_Frame*>& key_frames, std::vector<int>& interval) {
	int last_frame = 0;
	for (Key_Frame* key_frame : key_frames) {
		if (key_frame->frame_num == 0) { // avoid the starting zero, since all intervals start with frame 0
			continue;
		}
		interval.push_back(key_frame->frame_num - last_frame);
		last_frame = key_frame->frame_num;
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
		//if (gpu_frame_count == 1000) {
		//	break;
		//}
	}
	int p_x = vector_median(x);
	int p_y = vector_median(y);
	int width = std::min(vector_median(w), raw_size.width - p_x);
	int height = std::min(vector_median(h), raw_size.height - p_y);
	//if (1.0 * width / raw_size.width >= 0.95 && 1.0 * height / raw_size.height) {
	//	return cv::Rect();
	//}
	return cv::Rect(p_x, p_y, width, height);
}

#ifdef HAVE_OPENCV_CUDACODEC

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::cuda::GpuMat first_frame, cv::cuda::GpuMat second_frame) {
	Key_Frame* new_key_frame = new Key_Frame;
	new_key_frame->delta = delta;
	new_key_frame->frame_num = frame_num;
	new_key_frame->first_frame = first_frame;
	new_key_frame->second_frame = second_frame;
	key_frames.push_back(new_key_frame);
}

void frame_preprocessing(cv::cuda::GpuMat& frame) {
	cv::cuda::resize(frame, frame, cv::Size(128, 128));
	if (frame.channels() == 4) {
		cv::cuda::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
	}
	else if (frame.channels() == 3) {
		cv::cuda::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	}
}

void edge_detection(cv::cuda::GpuMat& frame, cv::Mat& edge_frame) {
	frame.download(edge_frame);
	cv::GaussianBlur(edge_frame, edge_frame, cv::Size(3, 3), 1, 1, cv::BORDER_DEFAULT);
	cv::Mat sobel_x, sobel_y;
	cv::Mat kernel_x = (cv::Mat_<double>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	cv::Mat kernel_y = (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	cv::filter2D(edge_frame, sobel_x, -1, kernel_x, cv::Point(-1, -1), 0, 4);
	cv::filter2D(edge_frame, sobel_y, -1, kernel_y, cv::Point(-1, -1), 0, 4);
	sobel_x.convertTo(sobel_x, CV_32FC1);
	sobel_y.convertTo(sobel_y, CV_32FC1);
	edge_frame.convertTo(edge_frame, CV_32FC1);
	cv::pow(sobel_x, 2, sobel_x);
	cv::pow(sobel_y, 2, sobel_y);
	cv::sqrt((sobel_x + sobel_y), edge_frame);
	edge_frame.convertTo(edge_frame, CV_8UC1);
}

#else

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::Mat first_frame, cv::Mat second_frame) {
	Key_Frame* new_key_frame = new Key_Frame;
	new_key_frame->delta = delta;
	new_key_frame->frame_num = frame_num;
	new_key_frame->first_frame = first_frame;
	new_key_frame->second_frame = second_frame;
	key_frames.push_back(new_key_frame);
}

void frame_preprocessing(cv::Mat& frame) {
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
	cv::Mat kernel_x = (cv::Mat_<double>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
	cv::Mat kernel_y = (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	cv::filter2D(edge_frame, sobel_x, -1, kernel_x, cv::Point(-1, -1), 0, 4);
	cv::filter2D(edge_frame, sobel_y, -1, kernel_y, cv::Point(-1, -1), 0, 4);
	sobel_x.convertTo(sobel_x, CV_32FC1);
	sobel_y.convertTo(sobel_y, CV_32FC1);
	edge_frame.convertTo(edge_frame, CV_32FC1);
	cv::pow(sobel_x, 2, sobel_x);
	cv::pow(sobel_y, 2, sobel_y);
	cv::sqrt((sobel_x + sobel_y), edge_frame);
	edge_frame.convertTo(edge_frame, CV_8UC1);
}

#endif