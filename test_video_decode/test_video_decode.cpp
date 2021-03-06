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
	std::vector<double> gpu_times;
	int gpu_frame_count = 0;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filepath.string());
	cv::cuda::GpuMat frame;
	cuda_reader->nextFrame(frame);
	while (true) {
		tm.reset(); tm.start();
		if (!cuda_reader->nextFrame(frame))
			break;
		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;
	}
#else
	cv::VideoCapture video_reader(filepath.string());
	cv::Mat frame;
	video_reader >> frame;
	while (true) {
		tm.reset(); tm.start();
		video_reader >> frame;
		if (frame.empty())
			break;
		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;
	}
#endif
	std::cout << std::endl << "Results:" << std::endl;
	std::sort(gpu_times.begin(), gpu_times.end());
	double gpu_avg = std::accumulate(gpu_times.begin(), gpu_times.end(), 0.0) / gpu_times.size();
	std::cout << "Perf : Avg : " << gpu_avg << " ms FPS : " << 1000.0 / gpu_avg << " Frames " << gpu_frame_count << std::endl;
	return 0;
}