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

class Interval_Comparison {
private:
	enum Direction {
		LEFT = -1,
		RIGHT = 1,
	};

	enum Status {
		NOT_MATCHED = 0,
		DIRECT_MATCH = 1,
		EXPAND_MATCH = 2,
		EXPAND_MATCH_BOUNDARY = 3,
	};

public:
	Interval_Comparison(const std::vector<double>& interval1, const std::vector<double>& interval2);
	void compare();
	int get_matched() const;
	void print_matched() const;

private:
	cv::Vec2i find_direct_match();
	bool expand(Direction direction);
	double explore(
		const std::vector<double>& interval,
		const std::vector<Status>& matched,
		int p, Direction direction) const;
	void mark(std::vector<Status>& matched, int p, Direction direction);

private:
	std::vector<double> interval1;
	std::vector<double> interval2;
	std::vector<Status> matched1;
	std::vector<Status> matched2;
	cv::Vec2i main_p;
};

#ifdef HAVE_OPENCV_CUDACODEC
/*
@brief Calculate the histogram of a grayscale image

@param img Input image in GPU Mat
@return Size 1 x 256 GPU Mat representing the histogram of input image.
The sum of the mat is 1
*/
cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img);

/*
@brief Calculate the Wasserstein distance between two histograms

@param hist1 Size N x 1 GPU Mat representing the histogram
@param hist2 Same size GPU Mat as hist1
@return Wasserstein distance of the two histograms
*/
double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2);

/*
@brief Calculate the direct pixel distance between two grayscale images

@param img1 Image 1 in GPU Mat
@param img2 Image 2 in GPU Mat. Same size as img1
@return Direct pixel distance between the two iamges
*/
int direct_distance(const cv::cuda::GpuMat& img1, const cv::cuda::GpuMat& img2);

void Radon_Transform(cv::cuda::GpuMat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle);
#endif
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

int interval_comparison_new(const std::vector<double>& v1, const std::vector<double>& v2);