#include <filesystem>
#include <opencv2/cudacodec.hpp>
#include <opencv2/highgui.hpp>

int main() {
	std::filesystem::path filepath = "../searcher/video.mp4";
	cv::Ptr<cv::cudacodec::VideoReader> cuda_reader = cv::cudacodec::createVideoReader(filepath.string());
	cv::cuda::GpuMat frame;
	cuda_reader->nextFrame(frame);
	while (true) {
		if (!cuda_reader->nextFrame(frame))
			break;
		cv::Mat tmp;
		frame.download(tmp);
		cv::imshow("frame", tmp );
		cv::waitKey(30);
	}
	cv::destroyAllWindows();
	return 0;
}