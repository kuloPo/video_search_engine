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

#include "common.h"
#include "utils.h"

void Radon_Transform(cv::Mat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle);

/*
@brief Calculate the Wasserstein distance between two histograms

@param hist1 Size N x 1 Mat representing the histogram
@param hist2 Same size Mat as hist1
@return Wasserstein distance of the two histograms
*/
double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2);

/*
@brief Calculate the number of matched interval of two vectors

@param v1, v2 Vectors to compare with
@return Number of matched interval found in v1
*/
int interval_comparison(const std::vector<double>& v1, const std::vector<double>& v2);

/*
@brief Calculate the Radon space distance between two grayscale images

@param img1 Image 1 in Mat
@param img2 Image 2 in Mat. Same size as img1
@return Radon space distance between the two iamges
*/
double radon_distance(const cv::Mat& radon1, const cv::Mat& radon2);
