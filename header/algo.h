#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <opencv2/cudacodec.hpp>
#include <opencv2/cudawarping.hpp>

#include "common.h"
#include "similar.h"

/*
@brief This function calculates the position of key frames of the given video
by comparing delta between frames.
The return result will contain the frame number, delta of the pairs, 
and reduced size frame images of the pairs. 
First and last frame of a video will always be returned.

@param filename Path of the video to create index
@return A vector containing the pointers of Key_Frame
*/
std::vector<Key_Frame*> create_index(const std::filesystem::path& filename);

/*
@brief This function calculates the interval (in frame number) of a video
given its vector of Key_Frame.

@param key_frames Vector containing the pointers of Key_Frame of the source video
@param interval Destination of the interval vector
*/
void calc_interval(const std::vector<Key_Frame*>& key_frames, std::vector<int>& interval);
