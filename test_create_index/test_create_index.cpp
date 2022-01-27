#include <filesystem>
#include <iostream>

#include "algo.h"
#include "common.h"
#include "similar.h"

int main() {
	std::filesystem::path filepath = "../searcher/video_flip.mp4";
	std::vector<Key_Frame*> key_frames = std::move(create_index(filepath));
	for (Key_Frame* key_frame : key_frames) {
		std::cout << key_frame->frame_num << " " << key_frame->delta << std::endl;
	}
	std::cout << std::endl;
	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}
	return 0;
}