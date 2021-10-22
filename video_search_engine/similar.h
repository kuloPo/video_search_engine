#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/shape/emdL1.hpp>

cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img);
double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2);
int direct_distance(const cv::cuda::GpuMat& img1, const cv::cuda::GpuMat& img2);