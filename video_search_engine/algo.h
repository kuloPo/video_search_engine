#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <opencv2/cudacodec.hpp>
#include <opencv2/cudawarping.hpp>

#include "common.h"
#include "similar.h"

std::vector<Key_Frame*> create_index(const std::filesystem::path& filename);
void calc_interval(const std::vector<Key_Frame*>& key_frames, std::vector<int>& interval);
void interval_merge(const std::vector<int>& interval, const int fps, std::vector<int>& interval_merged);
void interval_to_sec(const std::vector<int>& interval, const int fps, std::vector<double>& interval_sec);