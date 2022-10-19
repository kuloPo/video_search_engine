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

#include <iostream>
#include <numeric>
#include <filesystem>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/highgui.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#else
#include <opencv2/videoio.hpp>
#endif

int main() {
	std::filesystem::path filepath = "../rsrc/video.mp4";
	cv::TickMeter tm;
	std::vector<double> times;
	int frame_count = 0;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filepath.string());
#else
	cv::Mat frame;
	cv::VideoCapture video_reader(filepath.string());
#endif

	while (true) {
		tm.reset(); tm.start();
#ifdef HAVE_OPENCV_CUDACODEC
		if (!cuda_reader->nextFrame(frame))
			break;
#else 
		video_reader >> frame;
		if (frame.empty())
			break;
#endif
		tm.stop();
		times.push_back(tm.getTimeMilli());
		frame_count++;
	}

	std::sort(times.begin(), times.end());
	double time_avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
	std::cout << "Perf : Avg : " << time_avg << " ms FPS : " << 1000.0 / time_avg << " Frames " << frame_count << std::endl;
	return 0;
}