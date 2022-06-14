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
#include "algo.h"
#include "io.h"

int main() {
	read_config();
	cv::Mat img1 = cv::imread("1.png");
	cv::Mat img2 = cv::imread("2.png");
	cv::Mat radon1, edge_frame1, radon2, edge_frame2;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat src1, src2;
	src1.upload(img1);
	src2.upload(img2);
#else
	cv::Mat src1, src2;
	img1.copyTo(src1);
	img2.copyTo(src2);
#endif
	frame_preprocessing(src1);
	frame_preprocessing(src2);
	edge_detection(src1, edge_frame1);
	edge_detection(src2, edge_frame2);

	edge_frame1.convertTo(edge_frame1, CV_32FC1);
	edge_frame2.convertTo(edge_frame2, CV_32FC1);

	edge_frame1 /= sum(edge_frame1);
	edge_frame2 /= sum(edge_frame2);

	cout << sum(edge_frame1) << " " << sum(edge_frame2) << endl;

	Radon_Transform(edge_frame1, radon1, 45, 0, 180);
	Radon_Transform(edge_frame2, radon2, 45, 0, 180);
	double d = radon_distance(radon1, radon2);

	cout << d << endl;
	cout << average(radon1) << " " << average(radon2) << endl;
	cout << max(radon1) << " " << max(radon2) << endl;
	cv::waitKey();

	return 0;
}