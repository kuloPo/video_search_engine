#include "similar.h"

int min_matched_interval = 1500;
double interval_matching_epsilon = 0.5;

int main() {
	cv::Mat src(cv::Size(256, 256), CV_8UC1, cv::Scalar(255));
	cv::cuda::GpuMat src_gpu;
	src_gpu.upload(src);
	cv::Mat radon;
	RadonTransform(src_gpu, radon, 1, 0, 180);
	cv::normalize(radon, radon, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cout << radon.size() << endl;
	cv::imshow("", radon);
	cv::waitKey();
}