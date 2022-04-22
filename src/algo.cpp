#include "algo.h"

std::vector<Key_Frame*> create_index(const std::filesystem::path& filename) {
	cv::Mat first_radon, second_radon;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat first_frame, second_frame;
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filename.string());
	cuda_reader->nextFrame(first_frame);
#else
	cv::Mat first_frame, second_frame;
	cv::VideoCapture video_reader(filename.string());
	video_reader >> first_frame;
#endif
	frame_preprocessing(first_frame);

	//first_frame.download(first_frame_cpu);
	RadonTransform(first_frame, first_radon, 45, 0, 180);

	std::vector<Key_Frame*> key_frames;
	
	// add first frame into index
	add_key_frame(key_frames, 0, 0, first_frame, empty_frame);
	
	// variables for measuring performance
	cv::TickMeter tm;
	std::vector<double> gpu_times;
	int gpu_frame_count = 0;
	
	while (true) {
		tm.reset(); tm.start();
#ifdef HAVE_OPENCV_CUDACODEC
		if (!cuda_reader->nextFrame(second_frame))
			break;
#else 
		video_reader >> second_frame;
		if (second_frame.empty())
			break;
#endif
		// frame preprocessing
		frame_preprocessing(second_frame);

		// calculate histogram and the distance between hist
		RadonTransform(second_frame, second_radon, 45, 0, 180);
		double d = radon_distance(first_radon, second_radon);

		// If delta is greater threshold, write the information into vector
		if (d > frame_difference_threshold) {
			add_key_frame(key_frames, d, gpu_frame_count, first_frame, second_frame);
		}

		first_frame = second_frame;
		first_radon = second_radon;

		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;
	}
	
	// add last frame into index
	add_key_frame(key_frames, 0, gpu_frame_count, first_frame, empty_frame);
	
#if defined DEBUG_CREATE_INDEX || defined DEBUG_PERFORMANCE
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

#ifdef HAVE_OPENCV_CUDACODEC

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::cuda::GpuMat first_frame, cv::cuda::GpuMat second_frame) {
	Key_Frame* new_key_frame = new Key_Frame;
	new_key_frame->delta = delta;
	new_key_frame->frame_num = frame_num;
	new_key_frame->first_frame = first_frame;
	new_key_frame->second_frame = second_frame;
	key_frames.push_back(new_key_frame);
}

void frame_preprocessing(cv::cuda::GpuMat& frame) {
	cv::cuda::resize(frame, frame, cv::Size(128, 128));
	cv::cuda::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
}

#else

void add_key_frame(std::vector<Key_Frame*>& key_frames, int delta, int frame_num,
	cv::Mat first_frame, cv::Mat second_frame) {
	Key_Frame* new_key_frame = new Key_Frame;
	new_key_frame->delta = delta;
	new_key_frame->frame_num = frame_num;
	new_key_frame->first_frame = first_frame;
	new_key_frame->second_frame = second_frame;
	key_frames.push_back(new_key_frame);
}

void frame_preprocessing(cv::Mat& frame) {
	cv::resize(frame, frame, cv::Size(128, 128));
	cv::cvtColor(frame, frame, cv::COLOR_BGRA2GRAY);
}

#endif