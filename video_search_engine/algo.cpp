#include "algo.h"

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