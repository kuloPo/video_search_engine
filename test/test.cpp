#include <filesystem>
#include <iostream>
#include <opencv2/cudacodec.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/cudaimgproc.hpp>

void add_noise(cv::Mat src) {
	cv::Mat noise(src.size(), src.type());
	cv::randn(noise, 10, 20);
	src += noise;
}

void add_blank(cv::VideoWriter video_writer, cv::Size sz) {
	static int cnt = 0;
	if (cnt == 300) {
		cv::Mat tmp(sz, CV_8UC3, cv::Scalar(0, 0, 0));
		video_writer.write(tmp);
		cnt = 0;
		return;
	}
	cnt++;
}

void flip(cv::Mat src) {
	cv::flip(src, src, 1);
}

int main() {
	std::filesystem::path filepath = "../video_search_engine/video.mp4";
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filepath.string());
	cv::cuda::GpuMat frame;
	cv::Mat tmp;
	cuda_reader->nextFrame(frame);
	frame.download(tmp);
	flip(tmp);
	cv::VideoWriter video_writer("noise_video.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, tmp.size());
	video_writer.write(tmp);
	while (true) {
		if (!cuda_reader->nextFrame(frame))
			break;
		cv::cuda::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
		frame.download(tmp);
		//add_noise(tmp);
		//add_blank(video_writer, frame.size());
		//flip(tmp);
		//cv::imshow("frame", tmp);
		//cv::waitKey(30);
		video_writer.write(tmp);
	}
	//cv::destroyAllWindows();
	cuda_reader.release();
	video_writer.release();
	return 0;
}