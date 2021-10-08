#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <iostream>

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img);
double wasserstein_distance(const cv::Mat& img1, const cv::Mat& img2);