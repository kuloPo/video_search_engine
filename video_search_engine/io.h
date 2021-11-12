#pragma once

#define RAPIDJSON_HAS_STDSTRING 1

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <rapidjson/document.h>

#include "common.h"
#include "picosha2.h"

void show_image(const std::vector<Key_Frame*>& key_frames);
rapidjson::Value write_data(const std::filesystem::path& filename, const std::vector<Key_Frame*>& key_frames, rapidjson::Document::AllocatorType& allocator);

