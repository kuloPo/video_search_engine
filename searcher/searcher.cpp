#include <iostream>
#include <rapidjson/document.h>
#include <fstream>

#include "common.h"
#include "algo.h"
#include "utils.h"
#include "io.h"

std::unique_ptr<DB_Connector> DB;

/*
@brief Read ID of videos from inverted index

@param s CSV format string returned from database
@param search_range Destination vector of ID
*/
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

/*
@brief Read interval from database

@param s CSV format string returned from database
@param fps FPS of the video
@param interval Destination vector of interval
*/
void read_interval(std::string s, int fps, std::vector<double>& interval) {
	std::string delimiter = ",";

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		interval.push_back(1.0 * std::stoi(token) / fps);
		s.erase(0, pos + delimiter.length());
	}
}

void query(const std::filesystem::path& filename) {
	cout << filename << endl;
	int input_fps = get_fps(filename);
	// extract interval of the query video
	std::vector<Key_Frame*> key_frames = std::move(create_index(filename));

	std::vector<int> input_interval, interval_merged;
	std::vector<double> input_interval_sec;
	calc_interval(key_frames, input_interval);
	interval_merge(input_interval, input_fps, interval_merged);
	interval_to_sec(interval_merged, input_fps, input_interval_sec);

	// query interted index to find search range using each interval
	std::vector<std::string> search_range;

	for (double interval : input_interval_sec) {
		std::string search_sql = std::format("SELECT * FROM invert_index WHERE interval = {} OR interval = {}",
			std::to_string(std::floor(interval)), std::to_string(std::ceil(interval)));
		std::unique_ptr<pqxx::result>& query_result_floor = DB->performQuery(search_sql);
		if (!query_result_floor->empty()) {
			std::string ID = query_result_floor->begin()[1].as<std::string>();
			read_ID(ID, search_range);
		}
	}

	// print query video's interval and result from invert index
#ifdef DEBUG_SEARCHER
	cout << endl;
	cout << "Input FPS: " << input_fps << endl;
	cout << "Input inteval:" << endl;
	for (double interval : input_interval) {
		cout << interval << " ";
	}
	cout << endl;

	std::cout << "result from invert index:" << std::endl;

	for (std::string ID : search_range) {
		std::string search_sql = std::format("SELECT * FROM interval WHERE ID = '{}'", ID);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		cout << filename << endl;
	}

	std::cout << std::endl;

	cout << search_range.size() << " videos in search range\n" << endl;
#endif // DEBUG_SEARCHER

	std::cout << "result from interval matching:" << std::endl;

	// try to match each video in search range
	for (std::string ID : search_range) {
		std::string search_sql = std::format("SELECT * FROM interval WHERE ID = '{}'", ID);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		int fps = query_result->begin()[2].as<int>();
		std::string interval_str = query_result->begin()[3].as<std::string>();
		std::vector<double> interval_db;
		read_interval(interval_str, fps, interval_db);
		int similarity = interval_comparison(input_interval_sec, interval_db);
		double matched_percentage = 100.0 * similarity / input_interval_sec.size();
		if (matched_percentage >= min_matched_percentage) {
			std::cout << std::format("{}, matched interval: {}%\n", filename, matched_percentage);
		}
	}

	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}
}

int main() {
	DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);

	for (int i = 1; i <= 15; i++) {
		std::filesystem::path filename = std::format("D:\\datasets\\ST1\\ST1Query{}.mpeg", i);
		query(filename);
		cout << endl;
	}

	return 0;
}
