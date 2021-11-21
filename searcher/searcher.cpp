#include <iostream>
#include <rapidjson/document.h>
#include <fstream>

#include "common.h"
#include "algo.h"
#include "utils.h"
#include "io.h"

int main() {
	std::filesystem::path invert_index_path = index_path / "invert_index.json";
	std::filesystem::path interval_path = index_path / "interval.json";
	std::ifstream f;

	f.open(invert_index_path);
	std::string invert_index_json;
	getline(f, invert_index_json);
	f.close();
	rapidjson::Document invert_index;
	invert_index.Parse(invert_index_json.c_str());

	f.open(interval_path);
	std::string interval_json;
	getline(f, interval_json);
	f.close();
	rapidjson::Document interval_database;
	interval_database.Parse(interval_json.c_str());

	std::filesystem::path filename = "video.mp4";
	int input_fps = get_fps(filename);
	std::vector<Key_Frame*> key_frames = std::move(create_index(filename));
	
	std::vector<double> input_interval;
	int last_frame = 0;
	for (Key_Frame* key_frame : key_frames) {
		input_interval.push_back(1.0 * (key_frame->frame_num - last_frame) / input_fps);
		last_frame = key_frame->frame_num;
	}

	std::vector<std::string> search_range;

	for (int interval : input_interval) {
		std::string interval_str = std::to_string(interval);
		if (invert_index.HasMember(interval_str.c_str())){
			const rapidjson::Value& filename_array = invert_index[interval_str.c_str()];
			for (rapidjson::SizeType i = 0; i < filename_array.Size(); i++) {
				std::string filename = filename_array[i].GetString();
				if (!vector_contain(search_range, filename)) {
					search_range.push_back(filename);
				}
			}
		}
	}

	std::cout << "result from invert index:" << std::endl;

	for (std::string filename : search_range) {
		std::cout << filename << std::endl;
	}

	std::cout << std::endl;

	std::cout << "result from interval matching:" << std::endl;

	for (rapidjson::SizeType i = 0; i < interval_database.Size(); i++) {
		std::string filename = interval_database[i]["filename"].GetString();
		int fps = interval_database[i]["FPS"].GetInt();
		if (vector_contain(search_range, filename)) {
			const rapidjson::Value& interval_array = interval_database[i]["interval"];
			std::vector<double> interval;
			for (rapidjson::SizeType j = 0; j < interval_array.Size(); j++) {
				interval.push_back(1.0 * interval_array[j].GetInt() / fps);
			}
			int similarity = interval_comparison(interval, input_interval);
			if (similarity >= 5) {
				std::cout << filename << ", matched interval: " << similarity << std::endl;
			}
		}
	}

	return 0;
}