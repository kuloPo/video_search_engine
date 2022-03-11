#include "similar.h"
#include <iostream>

template<typename T> int interval_comparison(const std::vector<T>& v1, const std::vector<T>& v2) {
	int n1 = v1.size();
	int n2 = v2.size();
	cv::Mat m = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC1);
	/*
	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			int last = m.at<int>(x - 1, y - 1);
			int side = std::max(m.at<int>(x, y - 1), m.at<int>(x - 1, y));
			if (cv::abs(v1.at(x - 1) - v2.at(y - 1)) < 1) {
				m.at<int>(x, y) = std::max(last, side) + 1;
			}
			else {
				m.at<int>(x, y) = std::max(last, side);
			}
		}
	}*/

	int sum = 0;
	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (cv::abs(v1.at(x - 1) - v2.at(y - 1)) < 0.1) {
				m.at<int>(x, y) = m.at<int>(x - 1, y - 1) + 1;
			}
		}
	}
	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (m.at<int>(x, y) >= min_matched_interval && (x == n1 || y == n2 || m.at<int>(x+1, y+1) == 0)){
				sum += m.at<int>(x, y);
			}
		}
	}
	
	//for (int x = 1; x <= n1; x++) {
	//	for (int y = 1; y <= n2; y++) {
	//		if (m.at<int>(x, y) >=0) {
	//			std::cout<<m.at<T>(x, y)<<" ";
	//		}
	//		else {
	//			std::cout<<"  ";
	//		}
	//	}
	//	std::cout<<std::endl;
	//}
	
	//return m.at<int>(n1, n2);
	return sum;
}