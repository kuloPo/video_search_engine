#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "common.h"

void show_image(const std::vector<Key_Frame*>& key_frames);
void write_data(const std::filesystem::path& filename, const std::vector<Key_Frame*>& key_frames);