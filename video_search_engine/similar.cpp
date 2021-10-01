#include "similar.h"

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img) {
	cv::cuda::GpuMat hist;
	cv::cuda::calcHist(img, hist);
	hist.convertTo(hist, CV_32F);
	cv::cuda::divide(hist, cv::Scalar(img.rows * img.cols), hist);
	return hist;
}

double wasserstein_distance(const cv::Mat& img1, const cv::Mat& img2) {
	double last = 0;
	double sum = 0;
	for (int i = 0; i < 256; i++) {
		double A = img1.at<float>(i);
		double B = img2.at<float>(i);
		double EMDi = A + last - B;
		sum += std::abs(EMDi);
		last = EMDi;
	}
	return sum;
}