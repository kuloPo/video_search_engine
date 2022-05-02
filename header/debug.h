#pragma once

#include <vector>
#include <opencv2/highgui.hpp>
#include <opencv2/plot.hpp>
#include <opencv2/imgproc.hpp>

void plot(
	const std::string& name, 
	const cv::Mat& data, 
	const double max_h, 
	const double max_w, 
	const std::string& text = "");

void plot_delta(double new_data, int frame_num, double max_h);