#pragma once

#include <string>

inline const std::string DB_address = "127.0.0.1";
inline const std::string DB_port = "5432";
inline const std::string DB_user = "postgres";
inline const std::string DB_password = "123456";
inline const std::string DB_name = "MUSCLE_VCD_2007";

inline const std::filesystem::path video_path = "D:\\datasets\\MUSCLE_VCD_2007"; // Path of the folder containing source videos
inline const std::filesystem::path index_path = "D:\\MUSCLE_VCD_2007";           // Path for storing key frame images

inline const int frame_difference_threshold = 50; // Threshold of treating two adjacent frames as different frames 
inline const int min_matched_interval = 3;        // Minimum value can be returned by inverval_comparison(). 
												  // If the value is too low, queries with too few intervals may be wrongly matched
inline const int min_matched_percentage = 30;     // Queries with matched intervals above this threshold will be considered duplicate