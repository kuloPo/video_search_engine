#pragma once

#include <vector>
#include <numeric>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/shape/emdL1.hpp>
#include <opencv2/ximgproc/radon_transform.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#endif

#include "common.h"
#include "utils.h"

#ifdef HAVE_OPENCV_CUDACODEC
/*
@brief Calculate the histogram of a grayscale image

@param img Input image in GPU Mat
@return Size 1 x 256 GPU Mat representing the histogram of input image.
The sum of the mat is 1
*/
cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img);

/*
@brief Calculate the Wasserstein distance between two histograms

@param hist1 Size N x 1 GPU Mat representing the histogram
@param hist2 Same size GPU Mat as hist1
@return Wasserstein distance of the two histograms
*/
double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2);

/*
@brief Calculate the direct pixel distance between two grayscale images

@param img1 Image 1 in GPU Mat
@param img2 Image 2 in GPU Mat. Same size as img1
@return Direct pixel distance between the two iamges
*/
int direct_distance(const cv::cuda::GpuMat& img1, const cv::cuda::GpuMat& img2);

void RadonTransform(cv::cuda::GpuMat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle);
#endif
void RadonTransform(cv::Mat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle);

/*
@brief Calculate the Wasserstein distance between two histograms

@param hist1 Size N x 1 Mat representing the histogram
@param hist2 Same size Mat as hist1
@return Wasserstein distance of the two histograms
*/
double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2);

/*
@brief Calculate the number of matched interval of two vectors

@param v1, v2 Vectors to compare with
@return Number of matched interval found in v1
*/
int interval_comparison(const std::vector<double>& v1, const std::vector<double>& v2);

/*
@brief Calculate the Radon space distance between two grayscale images

@param img1 Image 1 in Mat
@param img2 Image 2 in Mat. Same size as img1
@return Radon space distance between the two iamges
*/
double radon_distance(const cv::Mat& radon1, const cv::Mat& radon2);