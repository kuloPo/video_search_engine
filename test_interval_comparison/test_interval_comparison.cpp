#include <filesystem>
#include <iostream>

#include "algo.h"
#include "common.h"
#include "similar.h"
#include "io.h"

int main() {
	std::filesystem::path filepath_1 = "../video_search_engine/video.mp4";
	std::filesystem::path filepath_2 = "../searcher/video_flip.mp4";

	std::vector<Key_Frame*> key_frames = std::move(create_index(filepath_1));

	std::cout << "video 1 key frame: " << std::endl;
	for (Key_Frame* key_frame : key_frames) {
		std::cout << key_frame->frame_num << " " << key_frame->delta << std::endl;
	}
	std::cout << std::endl;

	std::vector<double> interval_1;
	int last_frame = 0;
	int fps = get_fps(filepath_1);
	for (Key_Frame* key_frame : key_frames) {
		interval_1.push_back(1.0 * (key_frame->frame_num - last_frame) / fps);
		last_frame = key_frame->frame_num;
	}

	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}

	key_frames = std::move(create_index(filepath_2));

	std::cout << "video 2 key frame: " << std::endl;
	for (Key_Frame* key_frame : key_frames) {
		std::cout << key_frame->frame_num << " " << key_frame->delta << std::endl;
	}
	std::cout << std::endl;

	std::vector<double> interval_2;
	last_frame = 0;
	fps = get_fps(filepath_2);
	for (Key_Frame* key_frame : key_frames) {
		interval_2.push_back(1.0 * (key_frame->frame_num - last_frame) / fps);
		last_frame = key_frame->frame_num;
	}

	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}

	std::cout << "video 1 interval: " << std::endl;
	for (int i : interval_1) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	std::cout << "video 2 interval: " << std::endl;
	for (int i : interval_2) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	int similarity = interval_comparison(interval_1, interval_2);
	std::cout << std::endl << "similarity: " << similarity << std::endl;

	return 0;
}