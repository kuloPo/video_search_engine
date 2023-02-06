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

#include <filesystem>
#include <iostream>
#include <mutex>
#include <queue>

#include "algo.h"
#include "common.h"
#include "similar.h"
#include "utils.h"
#include "io.h"

std::queue<std::filesystem::path> working_queue;
std::mutex queue_mutex;

void thread_invoker(int deviceID) {
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::setDevice(deviceID);
#endif
	while (true) {
		queue_mutex.lock();
		if (working_queue.empty()) {
			queue_mutex.unlock();
			break;
		}
		std::filesystem::path filename = working_queue.front();
		working_queue.pop();
		queue_mutex.unlock();
		Keyframe_Detector detector(filename.string());
		detector.run();
		if (deviceID == 0) {
			detector.print_performance();
			cout << endl;
		}
	}
}

int main(int argc, char** argv) {
	read_config();
	std::filesystem::path filepath = "../rsrc/video.mp4";
	if (argc == 2) {
		filepath = argv[1];
	}

	// test single thread
	cout << "Single thread performance: \n";
	Keyframe_Detector keyframe_detector(filepath);
	keyframe_detector.run();
	keyframe_detector.print_performance();
	cout << endl;

	// test full thread 
	cout << "Full thread performance: \n";
	for (int i = 0; i < thread_num; i++) {
		working_queue.push(filepath);
	}
	std::vector<std::thread> thread_list;
	for (int i = 0; i < thread_num; i++) {
		std::thread t(thread_invoker, i);
		thread_list.push_back(std::move(t));
	}
	for (auto iter = thread_list.begin(); iter != thread_list.end(); iter++) {
		iter->join();
	}

	// test half-full thread 
	cout << "Half-full thread performance: \n";
	for (int i = 0; i < thread_num / 2; i++) {
		working_queue.push(filepath);
	}
	std::vector<std::thread> thread_list_half;
	for (int i = 0; i < thread_num / 2; i++) {
		std::thread t(thread_invoker, i);
		thread_list_half.push_back(std::move(t));
	}
	for (auto iter = thread_list_half.begin(); iter != thread_list_half.end(); iter++) {
		iter->join();
	}
	
	return 0;
}