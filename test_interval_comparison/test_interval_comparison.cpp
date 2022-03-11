#include <filesystem>
#include <iostream>

#include "algo.h"
#include "common.h"
#include "similar.h"
#include "io.h"

int main() {
	std::filesystem::path filepath_1 = "../searcher/video.mp4";
	std::filesystem::path filepath_2 = "../searcher/video_flip.mp4";

	std::vector<Key_Frame*> key_frames_1 = std::move(create_index(filepath_1));
	std::vector<Key_Frame*> key_frames_2 = std::move(create_index(filepath_2));

	std::cout << "video 1 key frame: " << std::endl;
	for (Key_Frame* key_frame : key_frames_1) {
		std::cout << key_frame->frame_num << " " << key_frame->delta << std::endl;
	}
	std::cout << std::endl;

	std::cout << "video 2 key frame: " << std::endl;
	for (Key_Frame* key_frame : key_frames_2) {
		std::cout << key_frame->frame_num << " " << key_frame->delta << std::endl;
	}
	std::cout << std::endl;

	std::vector<int> interval_1, interval_2;
	calc_interval(key_frames_1, interval_1);
	calc_interval(key_frames_2, interval_2);

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

	std::vector<double> interval_sec_1, interval_sec_2;
	interval_to_sec(interval_1, get_fps(filepath_1), interval_sec_1);
	interval_to_sec(interval_1, get_fps(filepath_2), interval_sec_2);

	int similarity = interval_comparison(interval_sec_1, interval_sec_2);
	std::cout << std::endl << "similarity: " << similarity << std::endl;

	for (Key_Frame* key_frame : key_frames_1) {
		delete key_frame;
	}

	for (Key_Frame* key_frame : key_frames_2) {
		delete key_frame;
	}

	return 0;
}