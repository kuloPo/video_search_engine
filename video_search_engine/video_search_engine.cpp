#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <numeric>
#include <vector>
#include <iomanip>
#include <stdio.h>
#include <opencv2/cudacodec.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "similar.h"
#include "algo.h"

inline const std::filesystem::path video_path = ".\\videos";
inline const std::filesystem::path csv_path = ".\\data\\interval.csv";
inline const std::filesystem::path key_frame_path = ".\\data";

struct Key_Frame {
	double delta;
	cv::cuda::GpuMat first_frame;
	cv::cuda::GpuMat second_frame;
	int frame_num;
};

void show_image(const std::vector<Key_Frame*>& key_frames) {
	for (Key_Frame* key_frame : key_frames) {
		printf("%d %.2f\n", key_frame->frame_num, key_frame->delta);
		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imshow("", tmp);
		cv::waitKey();
		key_frame->second_frame.download(tmp);
		cv::imshow("", tmp);
		cv::waitKey();
	}
}

void write_data(const std::filesystem::path& filename, const std::vector<Key_Frame*>& key_frames) {
	std::filesystem::create_directory(key_frame_path / filename);
	std::ofstream fs;
	fs.open(csv_path, std::ofstream::out | std::ofstream::app);
	fs << filename;
	for (Key_Frame* key_frame : key_frames) {
		std::string frame_num_str = std::to_string(key_frame->frame_num);
		fs << "," + frame_num_str;

		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imwrite((key_frame_path / filename / (frame_num_str + "_1.png")).string(), tmp);
		key_frame->second_frame.download(tmp);
		cv::imwrite((key_frame_path / filename / (frame_num_str + "_2.png")).string(), tmp);
	}
	fs << std::endl;
	fs.close();
}

std::vector<Key_Frame*> create_index(const std::filesystem::path& filename) {
	cv::cuda::GpuMat first_frame, second_frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filename.string());

	cuda_reader->nextFrame(first_frame);
	cv::cuda::resize(first_frame, first_frame, cv::Size(128, 128));
	cv::cuda::cvtColor(first_frame, first_frame, cv::COLOR_BGRA2GRAY);

	cv::TickMeter tm;
	std::vector<double> gpu_times;
	int gpu_frame_count = 0;

	std::vector<Key_Frame*> key_frames;

	while (true) {
		tm.reset(); tm.start();
		if (!cuda_reader->nextFrame(second_frame))
			break;

		cv::cuda::resize(second_frame, second_frame, cv::Size(128, 128));
		cv::cuda::cvtColor(second_frame, second_frame, cv::COLOR_BGRA2GRAY);

		cv::cuda::GpuMat hist1 = get_histogram(first_frame);
		cv::cuda::GpuMat hist2 = get_histogram(second_frame);
		cv::cuda::transpose(hist1, hist1);
		cv::cuda::transpose(hist2, hist2);

		double d = wasserstein_distance(hist1, hist2);
		if (d > 50) {
			Key_Frame* key_frame = new Key_Frame;
			key_frame->delta = d;
			key_frame->first_frame = first_frame;
			key_frame->second_frame = second_frame;
			key_frame->frame_num = gpu_frame_count;
			key_frames.push_back(key_frame);
		}

		first_frame = std::move(second_frame);

		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;

	}

	if (!gpu_times.empty())
	{
		std::cout << std::endl << "Results:" << std::endl;
		std::sort(gpu_times.begin(), gpu_times.end());
		double gpu_avg = std::accumulate(gpu_times.begin(), gpu_times.end(), 0.0) / gpu_times.size();
		std::cout << "GPU : Avg : " << gpu_avg << " ms FPS : " << 1000.0 / gpu_avg << " Frames " << gpu_frame_count << std::endl;
	}

	return key_frames;
}

int main() {
	std::fstream fs;
	fs.open(csv_path, std::ios::out);
	fs.close();

	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		std::filesystem::path filename = entry.path().filename();
		std::vector<Key_Frame*> key_frames = std::move(create_index(video_path / filename));
		write_data(filename, key_frames);
		for (Key_Frame* key_frame : key_frames) {
			delete key_frame;
		}
	}
		

	return 0;
}