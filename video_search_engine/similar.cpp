#include "similar.h"

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img) {
	cv::cuda::GpuMat hist;
	cv::cuda::calcHist(img, hist);
	hist.convertTo(hist, CV_32F);
	cv::cuda::divide(hist, cv::Scalar(img.rows * img.cols), hist);
	return hist;
}

double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2) {
	cv::Mat sig1, sig2;
	hist1.download(sig1);
	hist2.download(sig2);
	return cv::EMDL1(sig1, sig2);
}

double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2) {
	return cv::EMDL1(hist1, hist2);
}

int direct_distance(const cv::cuda::GpuMat& img1, const cv::cuda::GpuMat& img2) {
	cv::cuda::GpuMat tmp;
	cv::cuda::absdiff(img1, img2, tmp);
	return cv::cuda::sum(tmp)[0] / (img1.rows * img1.cols);
}

double radon_distance(const cv::Mat& img1, const cv::Mat& img2) {
    cv::Mat _img1, _img2;
    img1.copyTo(_img1);
    img2.copyTo(_img2);
    cv::resize(_img2, _img2, cv::Size(128, 128));
    cv::resize(_img2, _img2, cv::Size(128, 128));
    cv::Mat radon1;
    cv::Mat radon2;
    cv::ximgproc::RadonTransform(_img2, radon1, 1, 0, 180, true, false);
    cv::ximgproc::RadonTransform(_img2, radon2, 1, 0, 180, true, false);
    cv::normalize(radon1, radon1, 0, 1, cv::NORM_MINMAX, CV_32FC1);
    cv::normalize(radon2, radon2, 0, 1, cv::NORM_MINMAX, CV_32FC1);

    cv::Mat integral_distance(1, 180, CV_64FC1);
    for (int i = 0; i < 180; i++) {
        cv::Mat integral1 = radon1.col(i);
        cv::Mat integral2 = radon2.col(i);
        integral_distance.at<double>(0, i) = wasserstein_distance(integral1, integral2);

    }
    return cv::mean(integral_distance)[0];
}
