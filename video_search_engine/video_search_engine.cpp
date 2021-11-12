
#include <stdio.h>
#include <rapidjson/writer.h>

#include "similar.h"
#include "algo.h"
#include "io.h"
#include "common.h"
#include "algo.h"


int main() {
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value root(rapidjson::kArrayType);

	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		std::filesystem::path filename = entry.path().filename();
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
	//std::cout << buffer.GetString() << std::endl;
	std::ofstream fs(index_path);
	fs << buffer.GetString();
		

	return 0;
}