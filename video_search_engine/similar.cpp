#include "similar.h"

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img) {
	cv::cuda::GpuMat hist;
	cv::cuda::calcHist(img, hist);
	hist.convertTo(hist, CV_32F);
	cv::cuda::divide(hist, cv::Scalar(img.rows * img.cols), hist);
	return hist;
}

double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2) {
	double last = 0;
	double sum = 0;

	for (int i = 0; i < 256; i++) {
		double A = hist1.at<float>(i);
		double B = hist2.at<float>(i);
		double EMDi = A + last - B;
		sum += std::abs(EMDi);
		last = EMDi;
	}
	return sum;
}


//double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2) {
//	cv::Mat sig1(256, 2, CV_32FC1);
//	cv::Mat sig2(256, 2, CV_32FC1);
//	for (int i = 0; i < 256; i++) {
//		sig1.at<float>(i, 0) = hist1.at<float>(i);
//		sig1.at<float>(i, 1) = i;
//
//		sig2.at<float>(i, 0) = hist2.at<float>(i);
//		sig2.at<float>(i, 1) = i;
//	}
//	return 0;
//	return cv::EMD(sig1, sig2, cv::DIST_L2);
//}

