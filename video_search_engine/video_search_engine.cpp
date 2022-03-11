#include "similar.h"
#include "algo.h"
#include "io.h"
#include "common.h"

int main() {
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

			std::vector<Key_Frame*> key_frames = std::move(create_index(video_path / filename));
			std::string insert_sql = write_data(filename, key_frames);
			DB->performQuery(insert_sql);
			for (Key_Frame* key_frame : key_frames) {
				delete key_frame;
			}	
	}
	
	return 0;
}