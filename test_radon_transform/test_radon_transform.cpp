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

#include <opencv2/ximgproc/radon_transform.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using std::cout;
using std::endl;

int main() {
	cv::Mat src(cv::Size(256, 256), CV_8UC1, cv::Scalar(255));
	cv::Mat radon;
	cv::ximgproc::RadonTransform(src, radon, 1, 0, 180, false, true);
	cout << radon.size() << endl;
	cv::imshow("", radon);
	cv::waitKey();
}