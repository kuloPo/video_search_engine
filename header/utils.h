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

#include <vector>
#include <opencv2/core.hpp>

/*
@brief Merge all intervals that are less than one second. 
The too-short interval will be merged to the one before it. 
If it is the first interval in the vector, it will not be merged. 

@param interval Interval vector of the video
@param fps FPS of the video
@param interval_merged Destination of the merged interval vector
*/
void interval_merge(const std::vector<int>& interval, const int fps, std::vector<int>& interval_merged);

/*
@brief Convert interval of frame number into interval of second

@param interval Interval vector of the video
@param fps FPS of the video
@param interval_sec Destination of the converted interval vector
*/
void interval_to_sec(const std::vector<int>& interval, const int fps, std::vector<double>& interval_sec);

double max(const cv::Mat& m);

double min(const cv::Mat& m);

double average(const cv::Mat& m);

double sum(const cv::Mat& m);

/*
@brief Check if the element is inside a vector

@param vec Vector that may contain the element
@param elem Element that is searching for
@return true if contain. false else wise
*/
template <typename T> bool vector_contain(std::vector<T>& vec, const T& elem);

/*
@brief Find the maximum of 3 values. If two values are equal, return the one in the front. 

@param x, y, z Values to compare. The three values must be same type
@return The maximum value of the three inputs
*/
template <typename T> T max_3(T x, T y, T z);

/*
@brief Find the index of the maximum of 3 values. If two values are equal, return the one in the front.

@param x, y, z Values to compare. The three values must be same type
@return 0, 1, or 2
*/
template <typename T> int argmax_3(T x, T y, T z);
#include "utils.tpp"