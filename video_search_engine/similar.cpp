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

cv::Vec2i _path_back_trace(const cv::Mat& m, const cv::Mat& path_trace, const cv::Vec2i& path) {
	int x = path[0];
	int y = path[1];
	int path_index = argmax_3(m.at<int>(x - 1, y - 1), m.at<int>(x, y - 1), m.at<int>(x - 1, y));
	switch (path_index) {
	case 0:
		return path_trace.at<cv::Vec2i>(x - 1, y - 1);
	case 1:
		return path_trace.at<cv::Vec2i>(x, y - 1);
	case 2:
		return path_trace.at<cv::Vec2i>(x - 1, y);
	}
}

int interval_comparison(const std::vector<double>& v1, const std::vector<double>& v2) {
	int n1 = v1.size();
	int n2 = v2.size();
	cv::Mat m = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC1);
	cv::Mat path_trace = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC2);
	
	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (cv::abs(v1[x - 1] - v2[y - 1]) < 1) {
				m.at<int>(x, y) = m.at<int>(x - 1, y - 1) + 1;
				if (m.at<int>(x, y) > m.at<int>(x - 1, y) && m.at<int>(x, y) > m.at<int>(x, y - 1)) {
					path_trace.at<cv::Vec2i>(x, y) = cv::Vec2i(x, y);
				}
				else {
					path_trace.at<cv::Vec2i>(x, y) = _path_back_trace(m, path_trace, cv::Vec2i(x, y));
				}
			}
			else {
				m.at<int>(x, y) = max_3(m.at<int>(x - 1, y - 1), m.at<int>(x, y - 1), m.at<int>(x - 1, y));
				path_trace.at<cv::Vec2i>(x, y) = _path_back_trace(m, path_trace, cv::Vec2i(x, y));
			}
		}
	}

	std::vector<cv::Vec2i> paths;
	cv::Vec2i last_path = path_trace.at<cv::Vec2i>(n1, n2);
	paths.push_back(cv::Vec2i(n1 + 1, n2 + 1));
	while (last_path != cv::Vec2i(0, 0)) {
		paths.push_back(last_path);
		last_path = _path_back_trace(m, path_trace, last_path);
	}

	std::vector<int> matched_index;

	int last_1, last_2;
	last_1 = last_2 = 0;
	while (!paths.empty()) {
		cv::Vec2i path = paths.back();
		paths.pop_back();
		int x = path[0] - 1;
		int y = path[1] - 1;
		double sum_1, sum_2;
		sum_1 = sum_2 = 0;
		for (int i = last_1; i < x; i++) {
			sum_1 += v1[i];
		}
		for (int i = last_2; i < y; i++) {
			sum_2 += v2[i];
		}
		if (cv::abs(sum_1 - sum_2) < 1) {
			for (int i = last_1; i <= x; i++) {
				if (i < v1.size() && (matched_index.empty() || i != matched_index.back())) {
					matched_index.push_back(i);
				}
			}
		}
		last_1 = x;
		last_2 = y;
	}

	int sum, max, last;
	sum = 1;
	max = 0;
	last = -2;
	for (int i : matched_index) {
		if (i == last + 1) {
			sum++;
		}
		else {
			if (sum > max) {
				max = sum;
			}
			sum = 1;
		}
		last = i;
	}

#ifdef DEBUG_INTERVAL_COMPARISON
	for (int i : matched_index) {
		cout << i << " ";
	}
	cout << endl << endl;

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (m.at<int>(x, y) >=0) {
				std::cout<<m.at<int>(x, y)<<" ";
			}
			else {
				std::cout<<"  ";
			}
		}
		std::cout<<std::endl;
	}
	std::cout << std::endl;

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			cout << std::format("({},{}) ", path_trace.at<cv::Vec2i>(x, y)[0], path_trace.at<cv::Vec2i>(x, y)[1]);
		}
		std::cout << std::endl;
	}
#endif // DEBUG_INTERVAL_COMPARISON

	return sum >= min_matched_interval ? sum : 0;
}
