#include "utils.h"

void interval_merge(const std::vector<int>& interval, const int fps, std::vector<int>& interval_merged) {
	for (int i : interval) {
		if (i < fps && interval_merged.size() != 0) { // too small and not the first one
			int last_interval = interval_merged.back();
			interval_merged.pop_back();
			interval_merged.push_back(last_interval + i);
		}
		else {
			interval_merged.push_back(i);
		}
	}
}

void interval_to_sec(const std::vector<int>& interval, const int fps, std::vector<double>& interval_sec) {
	for (int i : interval) {
		interval_sec.push_back(1.0 * i / fps);
	}
}

double max(const cv::Mat& m) {
	double ret;
	cv::minMaxLoc(m, NULL, &ret);
	return ret;
}

double min(const cv::Mat& m) {
	double ret;
	cv::minMaxLoc(m, &ret, NULL);
	return ret;
}

double average(const cv::Mat& m) {
	return cv::mean(m)[0];
}

double sum(const cv::Mat& m) {
	return cv::sum(m)[0];
}