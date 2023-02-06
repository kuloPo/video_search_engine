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

#include "similar.h"

#include <numeric>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/shape/emdL1.hpp>
#include <opencv2/ximgproc/radon_transform.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#endif

double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2) {
	cv::Mat hist1_normed = hist1 / sum(hist1);
	cv::Mat hist2_normed = hist2 / sum(hist2);
	return cv::EMDL1(hist1_normed, hist2_normed);
}

void Radon_Transform(cv::Mat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle)
{
	cv::ximgproc::RadonTransform(src, dst, theta, start_angle, end_angle);
	dst.convertTo(dst, CV_32FC1);
}

cv::Vec2i _path_back_trace(const cv::Mat& m, const cv::Mat& path_trace, const cv::Vec2i& path) {
	int x = path[0];
	int y = path[1];
	int path_index = argmax_3(m.at<int>(x - 1, y - 1), m.at<int>(x, y - 1), m.at<int>(x - 1, y));
	switch (path_index) {
	case 0:
		return path_trace.at<cv::Vec2i>(x - 1, y - 1);
	case 1:
		return path_trace.at<cv::Vec2i>(x, y - 1);
	case 2:
		return path_trace.at<cv::Vec2i>(x - 1, y);
	default:
		return cv::Vec2i(-1, -1);
	}
}

int interval_comparison(const std::vector<double>& v1, const std::vector<double>& v2) {
	// use dynamic programming to find the number of matched interval
	int n1 = v1.size();
	int n2 = v2.size();
	cv::Mat m = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC1);
	cv::Mat path_trace = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC2);

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (cv::abs(v1[x - 1] - v2[y - 1]) < interval_matching_epsilon) { // difference smaller than epsilon, match found 
				m.at<int>(x, y) = m.at<int>(x - 1, y - 1) + 1; // update value
				// if this is the first match found on the same level, record its coordinate
				if (m.at<int>(x, y) > m.at<int>(x - 1, y) && m.at<int>(x, y) > m.at<int>(x, y - 1)) {
					path_trace.at<cv::Vec2i>(x, y) = cv::Vec2i(x, y);
				}
				else {
					path_trace.at<cv::Vec2i>(x, y) = _path_back_trace(m, path_trace, cv::Vec2i(x, y));
				}
			}
			else { // not match, inherit value 
				m.at<int>(x, y) = max_3(m.at<int>(x - 1, y - 1), m.at<int>(x, y - 1), m.at<int>(x - 1, y));
				path_trace.at<cv::Vec2i>(x, y) = _path_back_trace(m, path_trace, cv::Vec2i(x, y));
			}
		}
	}

	// coordinates representing the index of matched intervals
	std::vector<cv::Vec2i> paths;
	// start tracing from button right corner of mat
	cv::Vec2i last_path = path_trace.at<cv::Vec2i>(n1, n2);
	paths.push_back(cv::Vec2i(n1 + 1, n2 + 1));
	while (last_path != cv::Vec2i(0, 0)) {
		paths.push_back(last_path);
		last_path = _path_back_trace(m, path_trace, last_path);
	}

	// break intervals at where they are matched
	// sum up unmatched intervals in between and check if are equal
	// interval 1: 1 | 2 | 3       | 4 | 2.5 2.5 | 6
	// interval 2: 1 | 2 | 1.5 1.5 | 4 | 5       | 6
	std::vector<int> matched_index;

	int last_1, last_2;
	last_1 = last_2 = 0;
	while (!paths.empty()) {
		cv::Vec2i path = paths.back();
		paths.pop_back();
		int x = path[0] - 1;
		int y = path[1] - 1;
#ifdef DEBUG_INTERVAL_COMPARISON
		if (!paths.empty()) {
			printf("%d: %.2f, %d: %.2f\n", x, v1[x], y, v2[y]);
		}
#endif // DEBUG_INTERVAL_COMPARISON
		if (x - last_1 > 10 || y - last_2 > 10) {
			last_1 = x;
			last_2 = y;
			continue;
		}
		double sum_1, sum_2;
		sum_1 = sum_2 = 0;
		for (int i = last_1; i < x; i++) {
			sum_1 += v1[i];
		}
		for (int i = last_2; i < y; i++) {
			sum_2 += v2[i];
		}
		// prevent one very large interval happens to match the other
		// normal interval    : 1 2 3 4
		// very large interval: 1 ... 2 ... 3 ... 4
		if (cv::abs(sum_1 - sum_2) < interval_matching_epsilon) {
			for (int i = last_1; i <= x; i++) {
				if (i < v1.size() && (matched_index.empty() || i != matched_index.back())) {
					matched_index.push_back(i);
				}
			}
		}
		last_1 = x;
		last_2 = y;
	}

	// find consecutive sequence greater than threshold
	int sum, last, consecutive_count;
	sum = 0;
	last = -1;
	consecutive_count = 0;
	for (int i : matched_index) {
		if (last == -1) {
			consecutive_count = 1;
		}
		else if (i == last + 1) {
			consecutive_count++;
		}
		else {
			if (consecutive_count >= min_matched_interval) {
				sum += consecutive_count;
			}
			consecutive_count = 1;
		}
		last = i;
	}
	if (consecutive_count >= min_matched_interval) {
		sum += consecutive_count;
	}

#ifdef DEBUG_INTERVAL_COMPARISON
	cout << endl << endl;
	for (int i : matched_index) {
		cout << i << " ";
	}
	cout << endl << endl;

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (m.at<int>(x, y) >= 0) {
				cout << m.at<int>(x, y) << " ";
			}
			else {
				cout << "  ";
			}
		}
		cout << endl;
	}
	cout << endl;

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			printf("(%d,%d) ", path_trace.at<cv::Vec2i>(x, y)[0], path_trace.at<cv::Vec2i>(x, y)[1]);
		}
		cout << endl;
	}
#endif // DEBUG_INTERVAL_COMPARISON

	return sum;
	//return matched_index.size();
}

double radon_distance(const cv::Mat& radon1, const cv::Mat& radon2) {
	std::vector<double> integral_distance;
	for (int i = 0; i < 180 / Radon_theta; i++) {
		cv::Mat integral1 = radon1.col(i);
		cv::Mat integral2 = radon2.col(i);
		integral_distance.push_back(wasserstein_distance(integral1, integral2));
	}
	return std::accumulate(integral_distance.begin(), integral_distance.end(), 0.0) / integral_distance.size();
}
