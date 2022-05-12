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

#include "io.h"

std::mutex stdout_mutex;
std::mutex db_mutex;

DB_Connector::DB_Connector(const std::string& u,
	const std::string& h,
	const std::string& pswd,
	const std::string& db,
	const std::string& p)
	:user{ "user=" + u }, host{ " host=" + h }, password{ " password=" + pswd }, dbname{ " dbname=" + db }, port{ " port=" + p }
{
	initConnection(conn, user, host, password, dbname, port);
}

DB_Connector::~DB_Connector() {
	conn->close();
}

void DB_Connector::initConnection(std::unique_ptr<pqxx::connection>& c,
	const std::string& u,
	const std::string& h,
	const std::string& pswd,
	const std::string& db,
	const std::string& port)
{
	try {
		conn = std::make_unique<pqxx::connection>(user + host + password + dbname + port);
		std::cout << "Connected to " << conn->dbname() << '\n';
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		exit(1);
	}
}

std::unique_ptr<pqxx::result>& DB_Connector::performQuery(const std::string& query) {
	try {
		db_mutex.lock();
		trans = std::make_unique<pqxx::work>(*conn, "trans");
		res = std::make_unique<pqxx::result>(trans->exec(query));
		trans->commit();
		db_mutex.unlock();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		exit(1);
	}

	return res;
}

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

std::string form_search_sql(const std::string& ID) {
	std::string ret = "SELECT * from INTERVAL WHERE ID = '";
	ret += ID;
	ret += "';";
	return ret;
}

std::string form_insert_sql(
	const std::string& ID,
	const std::string& FILENAME,
	int FPS,
	const std::string& INTERVAL) {
	std::string ret = "INSERT INTO INTERVAL (ID,FILENAME,FPS,INTERVAL) VALUES ('";
	ret += ID;
	ret += "','";
	ret += FILENAME;
	ret += "',";
	ret += std::to_string(FPS);
	ret += ",'";
	ret += INTERVAL;
	ret += "');";
	return ret;
}

void safe_printf(const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);

	stdout_mutex.lock();
	vprintf(format, argptr);
	stdout_mutex.unlock();

	va_end(argptr);
}