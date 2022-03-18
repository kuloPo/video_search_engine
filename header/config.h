#pragma once

#include <string>

extern std::string DB_address;
extern std::string DB_port;
extern std::string DB_user;
extern std::string DB_password;
extern std::string DB_name;

extern std::filesystem::path video_path; // Path of the folder containing source videos
extern std::filesystem::path index_path; // Path for storing key frame images

extern int frame_difference_threshold;   // Threshold of treating two adjacent frames as different frames 
extern int min_matched_interval;         // Minimum value can be returned by inverval_comparison(). 
									     // If the value is too low, queries with too few intervals may be wrongly matched
extern int min_matched_percentage;       // Queries with matched intervals above this threshold will be considered duplicate