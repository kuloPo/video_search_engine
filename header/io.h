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

#pragma once

#include <string>
#include <vector>
#include <regex>
#include <stdarg.h>
#include <pqxx/pqxx>
#include <opencv2/opencv_modules.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#else
#include <opencv2/videoio.hpp>
#endif

#include "common.h"

class DB_Connector {
public:
    DB_Connector(
        const std::string& u,
        const std::string& h,
        const std::string& pswd,
        const std::string& db,
        const std::string& p);

    void initConnection(
        std::unique_ptr<pqxx::connection>& c,
        const std::string& u,
        const std::string& h,
        const std::string& pswd,
        const std::string& db,
        const std::string& port);

    std::unique_ptr<pqxx::result> performQuery(const std::string& query);

private:
    std::unique_ptr<pqxx::connection> conn;
    std::unique_ptr<pqxx::nontransaction> trans;

    std::string user;
    std::string host;
    std::string password;
    std::string dbname;
    std::string port;
};

class Video_Reader {
public:
    Video_Reader(const std::filesystem::path& filename);
    ~Video_Reader();
    void run();
    void print_performance();
protected:
    void init_video_reader();
    bool read_frame();
    virtual bool preprocess() { return true; };
    virtual void frame_operation() {};
    virtual void postprocess() {};

protected:
    std::filesystem::path filename;
    int frame_count;
    AutoMat frame;
    cv::TickMeter tm;
    std::vector<double> frame_time;

#ifdef HAVE_OPENCV_CUDACODEC
    cv::Ptr<cv::cudacodec::VideoReader> video_reader;
#else
    cv::VideoCapture video_reader;
#endif
};

/*
@brief Convert the interval vector of a video into a SQL insert query, together with other video information.  

@param interval Interval vector of the video
@param filename Path to the video
*/
std::string write_interval(const std::vector<int>& interval);

/*
@brief Write the key frame images to disk.

@param key_frames Vector containing the pointers of Key_Frame of the source video
@param path Destination folder to store the frame images
@param filename Path to the video
*/
//void write_key_frame(const std::vector<Key_Frame*>& key_frames, const std::filesystem::path& path, const std::filesystem::path& filename);

/*
@brief Get frame per second of the video.

@param filename Path to the video
@return fps of the video in int
*/
int get_fps(const std::filesystem::path& filename);

int get_total_frames(const std::filesystem::path& filename);

/*
@brief Initialize database. Call when program starts. 
*/
std::unique_ptr<DB_Connector> init_db();

std::string form_search_sql(const std::string& ID);

std::string form_insert_sql(
    const std::string& ID,
    const std::string& FILENAME,
    const int FPS,
    const std::string& INTERVAL);

void safe_printf(const char* format, ...);

std::string delete_db_data();

void read_config();

void hash_string(const std::string& str, std::string& dst);