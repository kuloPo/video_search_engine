#include "similar.h"

template<typename T> int interval_comparison(const std::vector<T>& v1, const std::vector<T>& v2) {
	int n1 = v1.size();
	int n2 = v2.size();
	cv::Mat m = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC1);

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (cv::abs(v1.at(x - 1) - v2.at(y - 1)) < 1) {
				m.at<int>(x, y) = m.at<int>(x - 1, y - 1) + 1;
			}
		}
	}

	double max_v;
	cv::minMaxLoc(m, NULL, &max_v);
	return (int)max_v;
}