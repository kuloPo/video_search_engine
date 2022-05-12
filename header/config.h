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

#pragma once

#include <string>

inline std::string DB_address = "127.0.0.1";
inline std::string DB_port = "5432";
inline std::string DB_user = "postgres";
inline std::string DB_password = "123456";
inline std::string DB_name = "MUSCLE_VCD_2007";

inline std::filesystem::path video_path = "D:\\datasets\\MUSCLE_VCD_2007"; // Path of the folder containing source videos
inline std::filesystem::path index_path = "D:\\MUSCLE_VCD_2007";           // Path for storing key frame images

inline double frame_difference_threshold = 0.00030;   // Threshold of treating two adjacent frames as different frames 
inline int min_matched_interval = 3;            // Minimum value can be returned by inverval_comparison(). 
   							                    // If the value is too low, queries with too few intervals may be wrongly matched
inline int min_matched_percentage = 30;         // Queries with matched intervals above this threshold will be considered duplicate
inline double interval_matching_epsilon = 0.5;  // difference smaller than epsilon, match found
inline int jumped_frame = 9;

inline int thread_num = 1;