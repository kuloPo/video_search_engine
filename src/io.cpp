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

#include "picosha2.h"
#include "ini.h"

std::string DB_address;
std::string DB_port;
std::string DB_user;
std::string DB_password;
std::string DB_name;

std::filesystem::path video_path;
double frame_difference_threshold;
int min_matched_interval;
int jumped_frame;
double interval_matching_epsilon;
int min_matched_percentage;
int thread_num;

std::string MUSCLE_VCD_2007;
std::string MUSCLE_VCD_2007_ST1;
std::string MUSCLE_VCD_2007_ST2;
std::string CC_WEB_VIDEO;
std::string NIST_TREC;
std::string NIST_TREC_query;

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

void DB_Connector::initConnection(std::unique_ptr<pqxx::connection>& c,
	const std::string& u,
	const std::string& h,
	const std::string& pswd,
	const std::string& db,
	const std::string& port)
{
	try {
		conn = std::make_unique<pqxx::connection>(user + host + password + dbname + port);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		exit(1);
	}
}

std::unique_ptr<pqxx::result> DB_Connector::performQuery(const std::string& query) {
	std::unique_ptr<pqxx::result> res;
	try {
		db_mutex.lock();
		trans = std::make_unique<pqxx::nontransaction>(*conn);
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

Video_Reader::Video_Reader(const std::filesystem::path& filename) {
	this->filename = filename;
	this->frame_count = 0;
	this->init_video_reader();
}

void Video_Reader::run() {
	if (!this->preprocess())
		return;
	while (true) {
		tm.reset();
		tm.start();
		if (!this->read_frame())
			break;

		frame_operation();

		tm.stop();
		frame_time.push_back(tm.getTimeMilli());
		frame_count++;
	}
	this->postprocess();
}

void Video_Reader::print_performance() {
	if (!frame_time.empty()) {
		std::sort(frame_time.begin(), frame_time.end());
		double total_time = std::accumulate(frame_time.begin(), frame_time.end(), 0.0);
		double avg = total_time / frame_count;
		safe_printf("%s %.2f %d %.2f\n", filename.filename().string().c_str(), total_time, frame_count, avg);
	}
}

void Video_Reader::init_video_reader() {
#ifdef HAVE_OPENCV_CUDACODEC
	cuda_reader = cv::cudacodec::createVideoReader(filename.string());
	this->read_frame();
#else
	video_reader = cv::VideoCapture(filename.string());
	this->read_frame();
#endif
}

bool Video_Reader::read_frame() {
#ifdef HAVE_OPENCV_CUDACODEC
	if (!cuda_reader->nextFrame(frame))
		return false;
#else 
	video_reader >> frame;
	return !frame.empty();
#endif
}

std::string write_interval(const std::vector<int>& interval) {
	std::string interval_str = "";
	// concatenate interva into string
	for (int i : interval) {
		std::string frame_interval_str = std::to_string(i);
		interval_str += frame_interval_str;
		interval_str += ",";
	}
	return interval_str;
}

int get_fps(const std::filesystem::path& filename) {
	int fps;
	cv::VideoCapture cap(filename.string());
	fps = cvRound(cap.get(cv::CAP_PROP_FPS));
	cap.release();
	return fps;
}

int get_total_frames(const std::filesystem::path& filename) {
	int frames;
	cv::VideoCapture cap(filename.string());
	frames = cvRound(cap.get(cv::CAP_PROP_FRAME_COUNT));
	cap.release();
	return frames;
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

std::string delete_db_data() {
	return "DELETE FROM interval; DELETE FROM invert_index;";
}

void read_config() {
	std::filesystem::path config_path;
	if (std::filesystem::exists("../rsrc/config.ini")) {
		config_path = "../rsrc/config.ini";
	}
	else if (std::filesystem::exists("./rsrc/config.ini")) {
		config_path = "./rsrc/config.ini";
	}
	else if (std::filesystem::exists("./config.ini")) {
		config_path = "./config.ini";
	}
	else {
		throw "config file not exists";
	}

	mINI::INIFile file(config_path.string());
	mINI::INIStructure ini;
	file.read(ini);

	DB_address = ini["Database"]["address"];
	DB_port = ini["Database"]["port"];
	DB_user = ini["Database"]["user"];
	DB_password = ini["Database"]["password"];
	DB_name = ini["Database"]["name"];

	video_path = ini["Filepath"]["video_path"];

	frame_difference_threshold = std::stod(ini["Search_Engine"]["frame_difference_threshold"]);
	min_matched_interval = std::stoi(ini["Search_Engine"]["min_matched_interval"]);
	jumped_frame = std::stoi(ini["Search_Engine"]["jumped_frame"]);

	interval_matching_epsilon = std::stod(ini["Searcher"]["interval_matching_epsilon"]);
	min_matched_percentage = std::stoi(ini["Searcher"]["min_matched_percentage"]);

	thread_num = std::stoi(ini["Performance"]["thread_num"]);

	MUSCLE_VCD_2007 = ini["Dataset"]["MUSCLE_VCD_2007"];
	MUSCLE_VCD_2007_ST1 = ini["Dataset"]["MUSCLE_VCD_2007_ST1"];
	MUSCLE_VCD_2007_ST2 = ini["Dataset"]["MUSCLE_VCD_2007_ST2"];
	CC_WEB_VIDEO = ini["Dataset"]["CC_WEB_VIDEO"];
	NIST_TREC = ini["Dataset"]["NIST_TREC"];
	NIST_TREC_query = ini["Dataset"]["NIST_TREC_query"];
}

void hash_string(const std::string& str, std::string& dst) {
	picosha2::hash256_hex_string(str, dst);
}