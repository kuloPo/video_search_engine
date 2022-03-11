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
		interval.push_back(1.0 * std::stoi(token) / fps);
		s.erase(0, pos + delimiter.length());
	}
	return interval;
}

int main() {
	std::unique_ptr<DB_Connector> DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);

	std::filesystem::path filename = "D:\\datasets\\ST1\\ST1Query5.mpeg";
	int input_fps = get_fps(filename);
	std::vector<Key_Frame*> key_frames = std::move(create_index(filename));
	
	std::vector<int> interval, interval_merged;
	std::vector<double> input_interval;
	calc_interval(key_frames, interval);
	interval_merge(interval, input_fps, interval_merged);
	interval_to_sec(interval_merged, input_fps, input_interval);

	cout << endl;
	cout << "Input FPS: " << input_fps << endl;
	cout << "Input inteval:" << endl;
	for (double interval : input_interval) {
		cout << interval << " ";
	}
	cout << endl;

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
		std::string search_sql = std::format("SELECT * FROM interval WHERE ID = '{}'", ID);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		cout << filename << endl;
	}

	std::cout << std::endl;

	cout << search_range.size() << " videos in search range\n" << endl;

	std::cout << "result from interval matching:" << std::endl;

	for (std::string ID : search_range) {
		std::string search_sql = std::format("SELECT * FROM interval WHERE ID = '{}'", ID);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		int fps = query_result->begin()[2].as<int>();
		std::string interval_str = query_result->begin()[3].as<std::string>();
		std::vector<double> interval = read_interval(interval_str, fps);
		int similarity = interval_comparison(interval, input_interval);
		if (similarity >= min_matched_interval) {
			std::cout << filename << ", matched interval: " << similarity << std::endl;
		}
	}

	return 0;
}
