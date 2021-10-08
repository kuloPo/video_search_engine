#include "similar.h"

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img) {
	cv::cuda::GpuMat hist;
	cv::cuda::calcHist(img, hist);
	hist.convertTo(hist, CV_32F);
	cv::cuda::divide(hist, cv::Scalar(img.rows * img.cols), hist);
	return hist;
}

//double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2) {
//	double last = 0;
//	double sum = 0;
//
//	for (int i = 0; i < 256; i++) {
//		double A = hist1.at<float>(i);
//		double B = hist2.at<float>(i);
//		double EMDi = A + last - B;
//		sum += std::abs(EMDi);
//		last = EMDi;
//	}
//	return sum;
//}


double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2) {
	cv::Mat sig1, sig2;
	hist1.download(sig1);
	hist2.download(sig2);
	return cv::EMDL1(sig1, sig2);
}

