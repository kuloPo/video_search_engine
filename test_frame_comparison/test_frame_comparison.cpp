#include "similar.h"
#include "algo.h"

int main() {
	cv::Mat img1 = cv::imread("1.png");
	cv::Mat img2 = cv::imread("2.png");
	cv::Mat radon1, edge_frame1, radon2, edge_frame2;
#ifdef HAVE_OPENCV_CUDACODEC
	cv::cuda::GpuMat src1, src2;
	src1.upload(img1);
	src2.upload(img2);
#else
	cv::Mat src1, src2;
	img1.copyTo(src1);
	img2.copyTo(src2);
#endif
	frame_preprocessing(src1);
	frame_preprocessing(src2);
	edge_detection(src1, edge_frame1);
	edge_detection(src2, edge_frame2);

	edge_frame1.convertTo(edge_frame1, CV_32FC1);
	edge_frame2.convertTo(edge_frame2, CV_32FC1);

	edge_frame1 /= sum(edge_frame1);
	edge_frame2 /= sum(edge_frame2);

	cout << sum(edge_frame1) << " " << sum(edge_frame2) << endl;

	RadonTransform(edge_frame1, radon1, 45, 0, 180);
	RadonTransform(edge_frame2, radon2, 45, 0, 180);
	double d = radon_distance(radon1, radon2);

	cout << d << endl;
	cout << average(radon1) << " " << average(radon2) << endl;
	cout << max(radon1) << " " << max(radon2) << endl;
	cv::waitKey();

	return 0;
}