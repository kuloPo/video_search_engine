#pragma once

#include <string>

inline const std::string DB_address = "127.0.0.1";
inline const std::string DB_port = "5432";
inline const std::string DB_user = "postgres";
inline const std::string DB_password = "123456";
inline const std::string DB_name = "video_search_engine";

inline const std::filesystem::path video_path = "..\\videos";
inline const std::filesystem::path index_path = "..\\data";

inline const int frame_difference_threshold = 50;
inline const int min_matched_interval = 3;