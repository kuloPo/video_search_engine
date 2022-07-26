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

extern std::string DB_address;
extern std::string DB_port;
extern std::string DB_user;
extern std::string DB_password;
extern std::string DB_name;

extern std::filesystem::path video_path; // Path of the folder containing source videos

extern double frame_difference_threshold;   // Threshold of treating two adjacent frames as different frames 
extern int min_matched_interval;            // Minimum value can be returned by inverval_comparison(). 
   							                    // If the value is too low, queries with too few intervals may be wrongly matched
extern int jumped_frame;

extern double interval_matching_epsilon;  // difference smaller than epsilon, match found
extern int min_matched_percentage;         // Queries with matched intervals above this threshold will be considered duplicate

extern int thread_num;

extern std::string MUSCLE_VCD_2007;
extern std::string MUSCLE_VCD_2007_ST1;
extern std::string MUSCLE_VCD_2007_ST2;
extern std::string CC_WEB_VIDEO;