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

int main(int argc, char** argv) {
	read_config();

	std::filesystem::path filepath_1 = "../rsrc/video.mp4";
	std::filesystem::path filepath_2 = "../rsrc/video_flip.mp4";

	if (argc == 3) {
		filepath_1 = argv[1];
		filepath_2 = argv[2];
	}

	std::vector<Key_Frame*> key_frames_1 = std::move(Keyframe_Detector(filepath_1).run());
	std::vector<Key_Frame*> key_frames_2 = std::move(Keyframe_Detector(filepath_2).run());

	cout << "video 1 key frame: " << endl;
	for (Key_Frame* key_frame : key_frames_1) {
		cout << key_frame->frame_num << " " << key_frame->delta << endl;
	}
	cout << endl;

	cout << "video 2 key frame: " << endl;
	for (Key_Frame* key_frame : key_frames_2) {
		cout << key_frame->frame_num << " " << key_frame->delta << endl;
	}
	cout << endl;

	int fps_1, fps_2;
	fps_1 = 25;
	fps_2 = 25;

	std::vector<int> interval_1, interval_2;
	std::vector<int> interval_merged_1, interval_merged_2;
	calc_interval(key_frames_1, interval_1);
	calc_interval(key_frames_2, interval_2);
	interval_merge(interval_1, fps_1, interval_merged_1);
	interval_merge(interval_2, fps_2, interval_merged_2);

	cout << "video 1 interval: " << endl;
	for (int i : interval_1) {
		cout << i << " ";
	}
	cout << endl;

	cout << "video 2 interval: " << endl;
	for (int i : interval_2) {
		cout << i << " ";
	}
	cout << endl;

	cout << endl;

	std::vector<double> interval_sec_1, interval_sec_2;
	interval_to_sec(interval_merged_1, fps_1, interval_sec_1);
	interval_to_sec(interval_merged_2, fps_2, interval_sec_2);

	cout << "video 1 interval in second: " << endl;
	for (double i : interval_sec_1) {
		cout << i << " ";
	}
	cout << endl;

	cout << "video 2 interval in second: " << endl;
	for (double i : interval_sec_2) {
		cout << i << " ";
	}
	cout << endl;

	cout << endl;

	int similarity = interval_comparison(interval_sec_1, interval_sec_2);
	double matched_percentage = 100.0 * similarity / interval_sec_1.size();
	printf("similarity: %f%%\n", matched_percentage);
	for (Key_Frame* key_frame : key_frames_1) {
		delete key_frame;
	}

	for (Key_Frame* key_frame : key_frames_2) {
		delete key_frame;
	}
	return 0;
}