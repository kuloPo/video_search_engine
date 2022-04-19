#include <filesystem>
#include <iostream>

#include "algo.h"
#include "common.h"
#include "similar.h"
#include "utils.h"

int frame_difference_threshold = 1500;
int min_matched_interval = 3;
double interval_matching_epsilon = 0.5;

int main() {
	std::filesystem::path filepath_1 = "../rsrc/video.mp4";
	std::filesystem::path filepath_2 = "../rsrc/video_flip.mp4";

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

	int fps_1, fps_2;
	fps_1 = 25;
	fps_2 = 25;

	std::vector<int> interval_1, interval_2;
	std::vector<int> interval_merged_1, interval_merged_2;
	calc_interval(key_frames_1, interval_1);
	calc_interval(key_frames_2, interval_2);
	interval_merge(interval_1, fps_1, interval_merged_1);
	interval_merge(interval_2, fps_2, interval_merged_2);

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

	std::cout << std::endl;

	std::vector<double> interval_sec_1, interval_sec_2;
	interval_to_sec(interval_merged_1, fps_1, interval_sec_1);
	interval_to_sec(interval_merged_2, fps_2, interval_sec_2);

	std::cout << "video 1 interval in second: " << std::endl;
	for (double i : interval_sec_1) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	std::cout << "video 2 interval in second: " << std::endl;
	for (double i : interval_sec_2) {
		std::cout << i << " ";
	}
	std::cout << std::endl;

	std::cout << std::endl;

	int similarity = interval_comparison(interval_sec_1, interval_sec_2);
	double matched_percentage = 100.0 * similarity / interval_sec_2.size();
	printf("similarity: %f%%\n", matched_percentage);
	for (Key_Frame* key_frame : key_frames_1) {
		delete key_frame;
	}

	for (Key_Frame* key_frame : key_frames_2) {
		delete key_frame;
	}
	return 0;
}