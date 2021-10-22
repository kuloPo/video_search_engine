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