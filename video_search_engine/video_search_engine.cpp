#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <vector>
#include <iomanip>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/cudacodec.hpp>
#include <opencv2/highgui.hpp>

#include "similar.h"
#include "algo.h"

cv::Mat read_img(const std::string& path) {
	cv::Mat img = cv::imread(path);
	cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	return img;
}

int main() {
	const std::string file = "video.mp4";

	cv::cuda::GpuMat first_frame, second_frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(file);

	cuda_reader->nextFrame(first_frame);
	cv::cuda::cvtColor(first_frame, first_frame, cv::COLOR_BGRA2GRAY);

	cv::TickMeter tm;
	std::vector<double> gpu_times;
	int gpu_frame_count = 0;

	//std::vector<std::pair<double, std::pair<cv::cuda::GpuMat, cv::cuda::GpuMat>>> distance;
	Sorted_Linked_List top_distance(10);

	while (true) {
		tm.reset(); tm.start();
		if (!cuda_reader->nextFrame(second_frame))
			break;
		cv::cuda::cvtColor(second_frame, second_frame, cv::COLOR_BGRA2GRAY);

		cv::cuda::GpuMat hist1 = get_histogram(first_frame);
		cv::cuda::GpuMat hist2 = get_histogram(second_frame);

		cv::Mat hist1_cpu, hist2_cpu;
		hist1.download(hist1_cpu);
		hist2.download(hist2_cpu);

		double d = wasserstein_distance(hist1_cpu, hist2_cpu);
		top_distance.insert(d, first_frame, second_frame);
		//distance.push_back(std::make_pair(d, std::make_pair(first_frame, second_frame)));
		//std::cout << std::fixed << std::setprecision(2)<<wasserstein_distance(hist1_cpu, hist2_cpu) << std::endl;

		first_frame = std::move(second_frame);
		
		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;

	}
	/*
	std::sort(distance.begin(), distance.end(), std::greater<double>());
	for (int i = 0; i < 10; i++) {
		std::cout << distance[i].first << " ";
	}
	std::cout << std::endl;
	*/
	top_distance.print_list();
	top_distance.show_img();

	if (!gpu_times.empty())
	{
		std::cout << std::endl << "Results:" << std::endl;

		std::sort(gpu_times.begin(), gpu_times.end());

		double gpu_avg = std::accumulate(gpu_times.begin(), gpu_times.end(), 0.0) / gpu_times.size();

		std::cout << "GPU : Avg : " << gpu_avg << " ms FPS : " << 1000.0 / gpu_avg << " Frames " << gpu_frame_count << std::endl;
	}

	return 0;
}