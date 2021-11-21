#include <stdio.h>
#include <iostream>
#include <rapidjson/writer.h>

#include "similar.h"
#include "algo.h"
#include "io.h"
#include "common.h"


int main() {
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value root(rapidjson::kArrayType);

	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		std::filesystem::path filename = entry.path().filename();
		std::cout << filename << std::endl;
		std::vector<Key_Frame*> key_frames = std::move(create_index(video_path / filename));
		rapidjson::Value video = write_data(filename, key_frames, allocator);
		root.PushBack(video, allocator);
		for (Key_Frame* key_frame : key_frames) {
			delete key_frame;
		}
	}

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	root.Accept(writer);
	std::ofstream fs(index_path / "interval.json");
	fs << buffer.GetString();
	
	return 0;
}