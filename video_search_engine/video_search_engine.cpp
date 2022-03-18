#include "similar.h"
#include "algo.h"
#include "io.h"
#include "common.h"

std::string DB_address;
std::string DB_port;
std::string DB_user;
std::string DB_password;
std::string DB_name;

std::filesystem::path video_path;
std::filesystem::path index_path;

int frame_difference_threshold;
int min_matched_interval;
int min_matched_percentage;

int main() {
	read_config("../rsrc/config.ini");
	std::unique_ptr<DB_Connector> DB = init_db();

	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		std::filesystem::path filename = entry.path().filename();
		std::cout << filename << std::endl;
		// check if already exist in db
		std::string ID;
		picosha2::hash256_hex_string(filename.string(), ID);
		std::string search_sql = std::format("SELECT * from INTERVAL WHERE ID = '{}'", ID);
		std::unique_ptr<pqxx::result>& query_result = DB->performQuery(search_sql);
		if (!query_result->empty()) {
			cout << "Already indexed" << endl;
			continue;
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
		std::string insert_sql = std::format("INSERT INTO INTERVAL (ID,FILENAME,FPS,INTERVAL) VALUES ('{}','{}',{},'{}');", 
			ID, filename_str, fps, interval_str);
		DB->performQuery(insert_sql);
		// write frame image to disk
		write_key_frame(key_frames, index_path, filename);
		// clean up
		for (Key_Frame* key_frame : key_frames) {
			delete key_frame;
		}	
	}
	
	return 0;
}