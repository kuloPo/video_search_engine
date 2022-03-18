#include "algo.h"

std::vector<Key_Frame*> create_index(const std::filesystem::path& filename) {
	cv::cuda::GpuMat first_frame, second_frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filename.string());

	// read the first frame, reduce its resolution and convert it into grayscale
	cuda_reader->nextFrame(first_frame);
	cv::cuda::resize(first_frame, first_frame, cv::Size(128, 128));
	cv::cuda::cvtColor(first_frame, first_frame, cv::COLOR_BGRA2GRAY);

	std::vector<Key_Frame*> key_frames;
	
	// add first frame into index
	Key_Frame* frame_zero = new Key_Frame;
	frame_zero->delta = 0;
	frame_zero->first_frame = first_frame;
	frame_zero->frame_num = 0;
	key_frames.push_back(frame_zero);
	
	// variables for measuring performance
	cv::TickMeter tm;
	std::vector<double> gpu_times;
	int gpu_frame_count = 0;
	
	while (cuda_reader->nextFrame(second_frame)) {
		tm.reset(); tm.start();
		
		// frame preprocessing
		cv::cuda::resize(second_frame, second_frame, cv::Size(128, 128));
		cv::cuda::cvtColor(second_frame, second_frame, cv::COLOR_BGRA2GRAY);

		// calculate histogram and the distance between hist
		cv::cuda::GpuMat hist1 = get_histogram(first_frame);
		cv::cuda::GpuMat hist2 = get_histogram(second_frame);
		cv::cuda::transpose(hist1, hist1);
		cv::cuda::transpose(hist2, hist2);
		double d = wasserstein_distance(hist1, hist2);

		// If delta is greater threshold, write the information into vector
		if (d > frame_difference_threshold) {
			Key_Frame* key_frame = new Key_Frame;
			key_frame->delta = d;
			key_frame->first_frame = first_frame;
			key_frame->second_frame = second_frame;
			key_frame->frame_num = gpu_frame_count;
			key_frames.push_back(key_frame);
		}

		first_frame = second_frame;

		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;

	}
	
	// add last frame into index
	Key_Frame* frame_last = new Key_Frame;
	frame_last->delta = 0;
	frame_last->first_frame = first_frame;
	frame_last->frame_num = gpu_frame_count;
	key_frames.push_back(frame_last);
	
#ifdef DEBUG_CREATE_INDEX
	if (!gpu_times.empty())
	{
		std::cout << std::endl << "Results:" << std::endl;
		std::sort(gpu_times.begin(), gpu_times.end());
		double gpu_avg = std::accumulate(gpu_times.begin(), gpu_times.end(), 0.0) / gpu_times.size();
		std::cout << "GPU : Avg : " << gpu_avg << " ms FPS : " << 1000.0 / gpu_avg << " Frames " << gpu_frame_count << std::endl;
	}
#endif // DEBUG_CREATE_INDEX
	
	return key_frames;
}

void calc_interval(const std::vector<Key_Frame*>& key_frames, std::vector<int>& interval) {
	int last_frame = 0;
	for (Key_Frame* key_frame : key_frames) {
		if (key_frame->frame_num == 0) { // avoid the starting zero, since all intervals start with frame 0
			continue;
		}
		interval.push_back(key_frame->frame_num - last_frame);
		last_frame = key_frame->frame_num;
	}
}
