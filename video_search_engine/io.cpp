#include "io.h"

void show_image(const std::vector<Key_Frame*>& key_frames) {
	for (Key_Frame* key_frame : key_frames) {
		printf("%d %.2f\n", key_frame->frame_num, key_frame->delta);
		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imshow("", tmp);
		cv::waitKey();
		key_frame->second_frame.download(tmp);
		cv::imshow("", tmp);
		cv::waitKey();
	}
}

std::string write_data(const std::filesystem::path& filename, const std::vector<Key_Frame*>& key_frames) {
	std::filesystem::create_directory(index_path / filename);
	std::string ID;
	picosha2::hash256_hex_string(filename.string(), ID);
	int fps = get_fps(video_path / filename);

	int last_frame = 0;
	std::string interval = "";

	for (Key_Frame* key_frame : key_frames) {
		std::string frame_num_str = std::to_string(key_frame->frame_num);
		std::string frame_interval_str = std::to_string(key_frame->frame_num - last_frame);
		interval += frame_interval_str;
		interval += ",";
		
		last_frame = key_frame->frame_num;

		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imwrite((index_path / filename / (frame_num_str + "_1.png")).string(), tmp);
		if (key_frame->delta != 0) { // not the first or last frame
			key_frame->second_frame.download(tmp);
			cv::imwrite((index_path / filename / (frame_num_str + "_2.png")).string(), tmp);
		}
	}

	std::string filename_str = filename.string();
	filename_str = std::regex_replace(filename_str, std::regex("'"), "''");
	std::string insert_sql = std::format("INSERT INTO INTERVAL (ID,FILENAME,FPS,INTERVAL) VALUES ('{}','{}',{},'{}');", ID, filename_str, fps, interval);
	cout << insert_sql << endl;
	return insert_sql;
}

int get_fps(const std::filesystem::path& filename) {
	int fps;
	cv::VideoCapture cap(filename.string());
	fps = cvRound(cap.get(cv::CAP_PROP_FPS));
	cap.release();
	return fps;
}

std::unique_ptr<DB_Connector> init_db() {
	std::unique_ptr<DB_Connector> DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
	std::string create_inverval_sql = "CREATE TABLE IF NOT EXISTS INTERVAL("  \
		"ID CHAR(64)    PRIMARY KEY     NOT NULL," \
		"FILENAME       TEXT    NOT NULL," \
		"FPS            SMALLSERIAL     NOT NULL," \
		"INTERVAL         TEXT NOT NULL);";
	DB->performQuery(create_inverval_sql);
	std::string create_invert_index_sql = "CREATE TABLE IF NOT EXISTS INVERT_INDEX("  \
		"INTERVAL INT    PRIMARY KEY     NOT NULL," \
		"ID         TEXT NOT NULL);";
	DB->performQuery(create_invert_index_sql);
	DB->performQuery("SET CLIENT_ENCODING TO 'WIN1252'");
	return DB;
}