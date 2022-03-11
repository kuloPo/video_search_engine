#include <filesystem>
#include <iostream>

#include "algo.h"
#include "common.h"
#include "similar.h"

#include <vector>

int main() {
	std::filesystem::path filepath = "D:\\datasets\\MUSCLE_VCD_2007\\movie99.mpg";
	std::vector<Key_Frame*> key_frames = std::move(create_index(filepath));
	for (Key_Frame* key_frame : key_frames) {
		std::cout << key_frame->frame_num << " " << key_frame->delta << std::endl;
	}
	std::cout << std::endl;
	std::vector<int> interval;
	std::vector<int> interval_merged;
	std::vector<double> interval_sec;
	int fps = 25;
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
	return 0;
}