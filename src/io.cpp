#include "io.h"


void show_image(const std::vector<Key_Frame*>& key_frames) {
	for (Key_Frame* key_frame : key_frames) {
		printf("%d %.2f\n", key_frame->frame_num, key_frame->delta);
#ifdef HAVE_OPENCV_CUDACODEC
		cv::Mat tmp_1, tmp_2;
		key_frame->first_frame.download(tmp_1);
		key_frame->second_frame.download(tmp_2);
		cv::imshow("", tmp_1);
		cv::waitKey();
		cv::imshow("", tmp_2);
		cv::waitKey();
#else 
		cv::imshow("", key_frame->first_frame);
		cv::waitKey();
		cv::imshow("", key_frame->second_frame);
		cv::waitKey();
#endif
	}
}


std::string write_interval(const std::vector<int>& interval, const std::filesystem::path& filename) {
	std::string ID;
	std::string interval_str = "";

	picosha2::hash256_hex_string(filename.string(), ID);
	std::filesystem::create_directory(index_path / filename);

	// concatenate interva into string
	for (int i : interval) {
		std::string frame_interval_str = std::to_string(i);
		interval_str += frame_interval_str;
		interval_str += ",";
	}
	return interval_str;
}

/*
void write_key_frame(const std::vector<Key_Frame*>& key_frames, const std::filesystem::path& path, const std::filesystem::path& filename) {
	for (Key_Frame* key_frame : key_frames) {
		cv::Mat tmp;
		std::string frame_num_str;
		key_frame->first_frame.download(tmp);
		frame_num_str = std::to_string(key_frame->frame_num);
		cv::imwrite((index_path / filename / (frame_num_str + "_1.png")).string(), tmp);
		if (key_frame->delta != 0) { // not the first or last frame
			key_frame->second_frame.download(tmp);
			cv::imwrite((index_path / filename / (frame_num_str + "_2.png")).string(), tmp);
		}
	}
}
*/

int get_fps(const std::filesystem::path& filename) {
	int fps;
	cv::VideoCapture cap(filename.string());
	fps = cvRound(cap.get(cv::CAP_PROP_FPS));
	cap.release();
	return fps;
}

std::unique_ptr<DB_Connector> init_db() {
	// Connect to database
	std::unique_ptr<DB_Connector> DB = std::make_unique<DB_Connector>(DB_user, DB_address, DB_password, DB_name, DB_port);
	// Create table interval and invert_index if not exist
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
