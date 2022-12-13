/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "common.h"
#include "algo.h"
#include "utils.h"
#include "io.h"
#include "similar.h"

std::unique_ptr<DB_Connector> DB;

class Keyframe_Detector_Demo_Searcher : public Keyframe_Detector {
public:
	Keyframe_Detector_Demo_Searcher(const std::filesystem::path& filename) : Keyframe_Detector(filename) {}

private:
	bool read_frame() {
		if (frame_count % 1000 == 0) {
			cout << frame_count << " frames completed" << endl;
		}
		return Keyframe_Detector::read_frame();
	}
};

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

std::string invert_index_search_sql(const int interval_floor, const int interval_ceil) {
	std::string ret = "SELECT * FROM invert_index WHERE interval = ";
	ret += std::to_string(interval_floor);
	ret += " OR interval = ";
	ret += std::to_string(interval_ceil);
	ret += ";";
	return ret;
}

void query(const std::filesystem::path& filename) {
	int input_fps = get_fps(filename);
	// extract interval of the query video
	std::vector<Key_Frame*> key_frames = std::move(Keyframe_Detector_Demo_Searcher(filename).run());

	std::vector<int> input_interval, interval_merged;
	std::vector<double> input_interval_sec;
	calc_interval(key_frames, input_interval);
	interval_merge(input_interval, input_fps, interval_merged);
	interval_to_sec(interval_merged, input_fps, input_interval_sec);

	// query interted index to find search range using each interval
	std::vector<std::string> search_range;

	for (double interval : input_interval_sec) {
		std::string search_sql = invert_index_search_sql(std::floor(interval), std::ceil(interval));
		std::unique_ptr<pqxx::result> query_result = DB->performQuery(search_sql);
		if (!query_result->empty()) {
			std::string ID = query_result->begin()[1].as<std::string>();
			read_ID(ID, search_range);
		}
	}

	std::vector<std::pair<double, std::string>> result;

	// try to match each video in search range
	for (std::string ID : search_range) {
		std::string search_sql = form_search_sql(ID);
		std::unique_ptr<pqxx::result> query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		int fps = query_result->begin()[2].as<int>();
		std::string interval_str = query_result->begin()[3].as<std::string>();
		std::vector<double> interval_db;
		read_interval(interval_str, fps, interval_db);
		int similarity = interval_comparison(input_interval_sec, interval_db);
		double matched_percentage = 100.0 * similarity / input_interval_sec.size();
		if (matched_percentage >= min_matched_percentage) {
			result.push_back(std::pair<double, std::string>(matched_percentage, filename));
		}
	}

	std::sort(result.begin(), result.end(), std::greater<std::pair<double, std::string>>());
	if (result.size() == 0) {
		cout << "The video you submitted is not found in the database" << endl;
	}
	else {
		printf("Video found in database. The Target video is %s with similarity %.2f\n", result[0].second.c_str(), result[0].first);
	}

	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}
}

int main(int argc, char** argv) {
	read_config();
	DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
	std::filesystem::path filepath;
	if (argc == 2) {
		filepath = argv[1];
	}
	else {
		cin >> filepath;
	}
	int total_frames = get_total_frames(filepath);
	cout << "Connection established" << endl;
	cout << filepath.filename() << " started" << endl;
	cout << "Total frames in this video: " << total_frames << endl;

	query(filepath);

	return 0;
}
