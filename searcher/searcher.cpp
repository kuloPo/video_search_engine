#include <iostream>
#include <rapidjson/document.h>
#include <fstream>

#include "common.h"
#include "algo.h"
#include "utils.h"
#include "io.h"

void read_ID(std::string s, std::vector<std::string>& search_range) {
	std::string delimiter = ",";

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		if (!vector_contain(search_range, token)) {
			search_range.push_back(token);
		}
		s.erase(0, pos + delimiter.length());
	}
}

std::vector<double> read_interval(std::string s, int fps) {
	std::vector<double> interval;
	std::string delimiter = ",";

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		interval.push_back(std::round(1.0 * std::stoi(token) / fps));
		s.erase(0, pos + delimiter.length());
	}
	return interval;
}

int main() {
	std::unique_ptr<DB_Connector> DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);

	std::filesystem::path filename = "video_flip.mp4";
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
		std::string search_sql = "SELECT * FROM invert_index WHERE interval = " + std::to_string(interval);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		if (!query_result->empty()) {
			std::string ID = query_result->begin()[1].as<std::string>();
			read_ID(ID, search_range);
		}
	}

	std::cout << "result from invert index:" << std::endl;

	for (std::string ID : search_range) {
		std::cout << ID << std::endl;
	}

	std::cout << std::endl;

	cout << search_range.size() << "videos in search range\n" << endl;

	std::cout << "result from interval matching:" << std::endl;

	//for (rapidjson::SizeType i = 0; i < interval_database.Size(); i++) {
	//	std::string filename = interval_database[i]["filename"].GetString();
	//	int fps = interval_database[i]["FPS"].GetInt();
	//	if (vector_contain(search_range, filename)) {
	//		const rapidjson::Value& interval_array = interval_database[i]["interval"];
	//		std::vector<double> interval;
	//		for (rapidjson::SizeType j = 0; j < interval_array.Size(); j++) {
	//			interval.push_back(1.0 * interval_array[j].GetInt() / fps);
	//		}
	//		int similarity = interval_comparison(interval, input_interval);
	//		if (similarity >= 5) {
	//			std::cout << filename << ", matched interval: " << similarity << std::endl;
	//		}
	//	}
	//}
	for (std::string ID : search_range) {
		std::string search_sql = std::format("SELECT * FROM interval WHERE ID = '{}'", ID);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		int fps = query_result->begin()[2].as<int>();
		std::string interval_str = query_result->begin()[3].as<std::string>();
		std::vector<double> interval = read_interval(interval_str, fps);
		int similarity = interval_comparison(interval, input_interval);
		if (similarity >= 5) {
			std::cout << filename << ", matched interval: " << similarity << std::endl;
		}
	}

	return 0;
}