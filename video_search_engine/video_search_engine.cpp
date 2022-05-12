/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.

 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include <queue>
#include <thread>

#include "similar.h"
#include "algo.h"
#include "io.h"
#include "common.h"

std::unique_ptr<DB_Connector> DB;
std::queue<std::filesystem::path> working_queue;
std::mutex queue_mutex;

void build_index(std::filesystem::path filename) {
	// check if already exist in db
	std::string ID;
	picosha2::hash256_hex_string(filename.string(), ID);
	std::string search_sql = form_search_sql(ID);
	std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
	if (!query_result->empty()) {
		safe_printf("%s Already indexed\n", filename.string().c_str());
		return;
	}

	// extract key frames
	std::vector<Key_Frame*> key_frames = std::move(create_index(video_path / filename));
	// get interval
	int fps = get_fps(video_path / filename);
	std::vector<int> interval, interval_merged;
	calc_interval(key_frames, interval);
	interval_merge(interval, fps, interval_merged);
	// write interval to database
	std::string interval_str = write_interval(interval_merged, filename);
	std::string filename_str = filename.string();
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
	while (working_queue.empty() == false) {
		queue_mutex.lock();
		std::filesystem::path filename = working_queue.front();
		working_queue.pop();
		queue_mutex.unlock();
		safe_printf("%s start. thread: %d\n", filename.string().c_str(), deviceID);
		build_index(filename);
	}
}

int main() {
	DB = init_db();
	
	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		working_queue.push(entry.path().filename());
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