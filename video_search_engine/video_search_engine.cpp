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

#include <queue>
#include <thread>

#include "common.h"
#include "similar.h"
#include "algo.h"
#include "io.h"

std::unique_ptr<DB_Connector> DB;
std::queue<std::filesystem::path> working_queue;
std::mutex queue_mutex;

void build_index(std::filesystem::path filepath) {
	// check if already exist in db
	std::string ID;
	hash_string(filepath.string(), ID);
	std::string search_sql = form_search_sql(ID);
	std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
	if (!query_result->empty()) {
		safe_printf("%s Already indexed\n", filepath.string().c_str());
		return;
	}

	// extract key frames
	std::vector<Key_Frame*> key_frames = std::move(create_index(filepath, MODE::INDEXER));
	// get interval
	int fps = get_fps(filepath);
	std::vector<int> interval, interval_merged;
	calc_interval(key_frames, interval);
	interval_merge(interval, fps, interval_merged);
	// write interval to database
	std::string interval_str = write_interval(interval_merged);
	std::string filename_str = filepath.filename().string();
	filename_str = std::regex_replace(filename_str, std::regex("'"), "''");
	std::string insert_sql = form_insert_sql(ID, filename_str, fps, interval_str);
	DB->performQuery(insert_sql);
	// write frame image to disk
	//write_key_frame(key_frames, index_path, filename);
	// clean up
	for (Key_Frame* key_frame : key_frames) {
		delete key_frame;
	}
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
		// safe_printf("%s start. thread: %d\n", filename.string().c_str(), deviceID);
		build_index(filename);
	}
}

int main() {
	cv::setNumThreads(0);
	read_config();
	DB = init_db();

	std::string delete_sql = delete_db_data();
	DB->performQuery(delete_sql);
	
	//for (const auto& entry : std::filesystem::directory_iterator(MUSCLE_VCD_2007)) {
	//	working_queue.push(entry.path());
	//}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(NIST_TREC)) {
		if (std::filesystem::is_regular_file(entry.path()) &&
			entry.path().stem().string().back() != '0') { // remove all videos end with 0 to simulate non-existing queries. 
			working_queue.push(entry.path());
		}
	}
	
	std::vector<std::thread> thread_list;
	for (int i = 0; i < thread_num; i++) {
		std::thread t(thread_invoker, i);
		thread_list.push_back(std::move(t));
	}
	
	for (auto iter = thread_list.begin(); iter != thread_list.end(); iter++) {
		iter->join();
	}

	return 0;
}