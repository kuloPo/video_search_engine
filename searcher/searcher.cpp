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
#include <fstream>
#include <algorithm>
#include <thread>
#include <queue>
#include <mutex>

#include "common.h"
#include "algo.h"
#include "utils.h"
#include "io.h"
#include "similar.h"

std::unique_ptr<DB_Connector> DB;
cv::TickMeter tm;
std::vector<double> search_times;

std::queue<std::filesystem::path> working_queue;
std::mutex queue_mutex;

class Keyframe_Detector_Searcher : public Keyframe_Detector {
public:
	Keyframe_Detector_Searcher(const std::filesystem::path& filename, double video_length_cut = 1.0) : Keyframe_Detector(filename) {
		bounding_box = find_bounding_box(filename);
		this->video_length_cut = video_length_cut;
	}

private:
	bool read_frame() {
		if (frame_count > total_frames * video_length_cut) {
			return false;
		}
		return Keyframe_Detector::read_frame();
	}

#ifdef HAVE_OPENCV_CUDACODEC
	void frame_process(cv::cuda::GpuMat& in_frame, cv::Mat& out_frame) {
#else
	void frame_process(cv::Mat & in_frame, cv::Mat & out_frame) {
#endif
		in_frame = in_frame(bounding_box);
		Keyframe_Detector::frame_process(in_frame, out_frame);
	}

private:
	cv::Rect bounding_box;
	double video_length_cut;
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

void write_result(
	std::string& result_str,
	const std::filesystem::path& filename,
	const std::pair<double, std::string>& result
) {
	if (!result_str.empty()) {
		result_str += "\n";
	}
	result_str += filename.filename().string();
	result_str += " ";
	result_str += result.second;
	result_str += " ";
	result_str += std::to_string(result.first);
	result_str += "%";
}

std::string query(const std::filesystem::path& filename, bool print_full = false, double video_length_cut = 1.0) {
	int input_fps = get_fps(filename);
	// extract interval of the query video
	//std::vector<Key_Frame*> key_frames = std::move(create_index(filename, MODE::SEARCHER, video_length_cut));
	std::vector<Key_Frame*> key_frames = std::move(Keyframe_Detector_Searcher(filename, video_length_cut).run());

	std::vector<int> input_interval, interval_merged;
	std::vector<double> input_interval_sec;
	calc_interval(key_frames, input_interval);
	interval_merge(input_interval, input_fps, interval_merged);
	interval_to_sec(interval_merged, input_fps, input_interval_sec);

	tm.reset(); tm.start();

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
		std::string search_sql = form_search_sql(ID);
		std::unique_ptr<pqxx::result> query_result = DB->performQuery(search_sql);
		std::string filename = query_result->begin()[1].as<std::string>();
		cout << filename << endl;
	}

	std::cout << std::endl;

	cout << search_range.size() << " videos in search range\n" << endl;
	std::cout << "result from interval matching:" << std::endl;
#endif // DEBUG_SEARCHER
	
	std::vector<std::pair<double, std::string>> result;

	// try to match each video in search range
	for (std::string ID : search_range) {
		std::string search_sql = form_search_sql(ID);
		std::unique_ptr<pqxx::result> query_result = DB->performQuery(search_sql);
		std::string videoname = query_result->begin()[1].as<std::string>();
		int fps = query_result->begin()[2].as<int>();
		std::string interval_str = query_result->begin()[3].as<std::string>();
		std::vector<double> interval_db;
		read_interval(interval_str, fps, interval_db);
		int similarity = interval_comparison(input_interval_sec, interval_db);
		double matched_percentage = 100.0 * similarity / input_interval_sec.size();
		if (filename.filename().string() != videoname && 
			matched_percentage >= min_matched_percentage) {
			result.push_back(std::pair<double, std::string>(matched_percentage, videoname));
		}
	}

	std::sort(result.begin(), result.end(), std::greater<std::pair<double, std::string>>());
	if (result.size() == 0) {
		result.push_back(std::pair<double, std::string>(0, "not_in_db"));
	}
	std::string search_result = "";
	write_result(search_result, filename, result[0]);
	if (print_full) {
		for (int i = 1; i < result.size(); i++) {
			write_result(search_result, filename, result[i]);
		}
	}

	tm.stop();
	search_times.push_back(tm.getTimeMilli());

	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}

	return search_result;
}

void thread_invoker(int deviceID) {
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::setDevice(deviceID);
#endif
	while (true) {
		queue_mutex.lock();
		if (working_queue.empty()) {
			queue_mutex.unlock();
			break;
		}
		std::filesystem::path filename = working_queue.front();
		working_queue.pop();
		queue_mutex.unlock();
		std::string result = query(filename.string());
		safe_printf("%s\n", result.c_str());
	}
}

int main() {
	read_config();
	DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);

	std::vector<std::string> search_result(15);
	parallel_for_(cv::Range(1, 15), [&](const cv::Range& range) {
		for (int i = range.start; i <= range.end; i++) {
			std::filesystem::path filename = std::filesystem::path(MUSCLE_VCD_2007_ST1) / ("ST1Query" + std::to_string(i) + ".mpeg");
			search_result[i - 1] = query(filename, false, 0.5);
		}
	}, thread_num);
	for (std::string result : search_result) {
		cout << result << endl;
	}

	//std::vector<std::string> search_result(3);
	//parallel_for_(cv::Range(1, 3), [&](const cv::Range& range) {
	//	for (int i = range.start; i <= range.end; i++) {
	//		std::filesystem::path filename = std::filesystem::path(MUSCLE_VCD_2007_ST2) / ("ST2Query" + std::to_string(i) + ".mpg");
	//		search_result[i - 1] = query(filename, true);
	//	}
	//}, thread_num);
	//for (std::string result : search_result) {
	//	cout << result << endl;
	//}

	//for (const auto& entry : std::filesystem::directory_iterator(CC_WEB_VIDEO)) {
	//	working_queue.push(entry.path());
	//}
	//std::vector<std::thread> thread_list;
	//for (int i = 0; i < thread_num; i++) {
	//	std::thread t(thread_invoker, i);
	//	thread_list.push_back(std::move(t));
	//}
	//for (auto iter = thread_list.begin(); iter != thread_list.end(); iter++) {
	//	iter->join();
	//}

	//for (const auto& entry : std::filesystem::directory_iterator(NIST_TREC_query)) {
	//	working_queue.push(entry.path());
	//}
	//std::vector<std::thread> thread_list;
	//for (int i = 0; i < thread_num; i++) {
	//	std::thread t(thread_invoker, i);
	//	thread_list.push_back(std::move(t));
	//}
	//for (auto iter = thread_list.begin(); iter != thread_list.end(); iter++) {
	//	iter->join();
	//}

	//std::sort(search_times.begin(), search_times.end());
	//double time_avg = std::accumulate(search_times.begin(), search_times.end(), 0.0) / search_times.size();
	//printf("Average search time: %.f ms\n", time_avg);

	return 0;
}
