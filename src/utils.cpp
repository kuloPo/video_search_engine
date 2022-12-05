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

#include "utils.h"

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

void interval_merge(const std::vector<int>& interval, const int fps, std::vector<int>& interval_merged) {
	for (int i : interval) {
		if (i < fps && interval_merged.size() != 0) { // too small and not the first one
			int last_interval = interval_merged.back();
			interval_merged.pop_back();
			interval_merged.push_back(last_interval + i);
		}
		else {
			interval_merged.push_back(i);
		}
	}
}

void interval_to_sec(const std::vector<int>& interval, const int fps, std::vector<double>& interval_sec) {
	for (int i : interval) {
		interval_sec.push_back(1.0 * i / fps);
	}
}

double max(const cv::Mat& m) {
	double ret;
	cv::minMaxLoc(m, NULL, &ret);
	return ret;
}

double min(const cv::Mat& m) {
	double ret;
	cv::minMaxLoc(m, &ret, NULL);
	return ret;
}

double average(const cv::Mat& m) {
	return cv::mean(m)[0];
}

double sum(const cv::Mat& m) {
	return cv::sum(m)[0];
}

#ifdef HAVE_OPENCV_CUDACODEC
void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::cuda::GpuMat first_frame, cv::cuda::GpuMat second_frame) {
	cv::Mat first_frame_cpu, second_frame_cpu;
	if (!first_frame.empty()) {
		first_frame.download(first_frame_cpu);
	}
	if (!second_frame.empty()) {
		second_frame.download(second_frame_cpu);
	}
	add_key_frame(key_frames, delta, frame_num, first_frame_cpu, second_frame_cpu);
}
#endif

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::Mat first_frame, cv::Mat second_frame) {
	Key_Frame* new_key_frame = new Key_Frame;
	new_key_frame->delta = delta;
	new_key_frame->frame_num = frame_num;
	new_key_frame->first_frame = first_frame;
	new_key_frame->second_frame = second_frame;
	key_frames.push_back(new_key_frame);
}