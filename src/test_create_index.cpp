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

#include "algo.h"
#include "common.h"
#include "similar.h"
#include "utils.h"
#include "io.h"

#include <vector>

int main(int argc, char** argv) {
	read_config();
	std::filesystem::path filepath = "../rsrc/video.mp4";
	if (argc == 2) {
		filepath = argv[1];
	}
	Keyframe_Detector keyframe_detector(filepath);
	keyframe_detector.run();
	keyframe_detector.print_performance();
	std::vector<Key_Frame*> key_frames = std::move(keyframe_detector.get_index());
	for (Key_Frame* key_frame : key_frames) {
		cout << key_frame->frame_num << " " << key_frame->delta << endl;
	}
	cout << endl;
	std::vector<int> interval;
	std::vector<int> interval_merged;
	std::vector<double> interval_sec;
	int fps = get_fps(filepath);
	calc_interval(key_frames, interval);
	interval_merge(interval, fps, interval_merged);
	interval_to_sec(interval_merged, fps, interval_sec);
	cout << "interval in frame: \n";
	for (int i : interval) {
		cout << i << " ";
	}
	cout << "\n\n";
	cout << "merged interval in frame: \n";
	for (int i : interval_merged) {
		cout << i << " ";
	}
	cout << "\n\n";
	cout << "interval in second: \n";
	for (double i : interval_sec) {
		cout << i << " ";
	}
	cout << "\n\n";
	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}
	double accumulate = 0;
	cout << "interval in accumulate second: \n";
	for (double i : interval_sec) {
		accumulate += i;
		cout << accumulate << " ";
	}
	cout << "\n\n";
	return 0;
}
