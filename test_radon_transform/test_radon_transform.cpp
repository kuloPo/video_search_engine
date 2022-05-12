/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.

 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include "similar.h"

int main() {
	cv::Mat src(cv::Size(256, 256), CV_8UC1, cv::Scalar(255));
	cv::Mat radon;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat src_gpu;
	src_gpu.upload(src);
	RadonTransform(src_gpu, radon, 1, 0, 180);
#else
	RadonTransform(src, radon, 1, 0, 180);
#endif
	cv::normalize(radon, radon, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cout << radon.size() << endl;
	cv::imshow("", radon);
	cv::waitKey();
}