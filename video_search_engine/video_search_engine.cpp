
#include <stdio.h>

#include "similar.h"
#include "algo.h"
#include "io.h"
#include "common.h"
#include "algo.h"


int main() {
	std::fstream fs;
	fs.open(csv_path, std::ios::out);
	fs.close();

	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		std::filesystem::path filename = entry.path().filename();
		std::vector<Key_Frame*> key_frames = std::move(create_index(video_path / filename));
		write_data(filename, key_frames);
		for (Key_Frame* key_frame : key_frames) {
			delete key_frame;
		}
	}
		

	return 0;
}