#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/shape/emdL1.hpp>

cv::Mat create_cost() {
	cv::Mat cost(256, 256, CV_32FC1);
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			cost.at<float>(i, j) = abs(i - j);
		}
	}
	return cost;
}

float calc_distance(int d1, int d2) {
	int x1, x2, y1, y2;
	x1 = d1 / 32;
	y1 = d1 % 32;
	x2 = d2 / 32;
	y2 = d2 % 32;
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

cv::Mat create_cost_2d() {
	cv::Mat cost(1024, 1024, CV_32FC1);
	for (int i = 0; i < 1024; i++) {
		for (int j = 0; j < 1024; j++) {
			cost.at<float>(i, j) = calc_distance(i, j);
		}
	}
	return cost;
}

int main() {
	cv::Mat img1 = cv::imread("1.png");
	cv::Mat img2 = cv::imread("2.png");

	cv::cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(img2, img2, cv::COLOR_BGR2GRAY);

	cv::resize(img1, img1, cv::Size(128, 128));
	cv::resize(img2, img2, cv::Size(128, 128));

	//cv::Rect rng(64, 64, 32, 32);
	//img1 = img1(rng);
	//img2 = img2(rng);

	//cv::imshow("1", img1);
	//cv::imshow("2", img2);
	//cv::waitKey();

	cv::Mat sig1(32 * 32, 1, CV_32FC1);
	cv::Mat sig2(32 * 32, 1, CV_32FC1);
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			sig1.at<float>(i * 32 + j, 0) = (float)img1.at<uchar>(i, j);
			//sig1.at<float>(i * 32 + j, 1) = i;
			//sig1.at<float>(i * 32 + j, 2) = j;

			sig2.at<float>(i * 32 + j, 0) = (float)img2.at<uchar>(i, j);
			//sig2.at<float>(i * 32 + j, 1) = i;
			//sig2.at<float>(i * 32 + j, 2) = j;
		}
	}


	cv::Mat cost = create_cost_2d();
	//for (int i = 0; i < 1024; i++) {
	//	float tmp = cost.at<float>(0, i);
	//	if (i % 32 == 0) {
	//		std::cout << std::endl;
	//	}
	//	std::cout << tmp << " ";
	//	
	//}
	std::cout << cv::EMD(sig1, sig2, cv::DIST_USER, cost) << std::endl;

	return 0;
}