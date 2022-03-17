#pragma once

#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/shape/emdL1.hpp>
#include <opencv2/ximgproc/radon_transform.hpp>
#include "common.h"
#include "utils.h"

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img);
double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2);
double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2);
int direct_distance(const cv::cuda::GpuMat& img1, const cv::cuda::GpuMat& img2);
double radon_distance(const cv::Mat& img1, const cv::Mat& img2);
int interval_comparison(const std::vector<double>& v1, const std::vector<double>& v2);