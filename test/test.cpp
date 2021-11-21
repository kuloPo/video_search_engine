#include <iostream>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <string>

using namespace std;

const std::filesystem::path video_path = "D:\\drive\\programs\\Download Youtube Video\\videos";

int main() {
	for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
		std::filesystem::path filename = entry.path().filename();
		std::cout << filename << std::endl;
	//	cv::VideoCapture cap(filename.string(), cv::CAP_FFMPEG);
	//	double ex = cap.get(cv::CAP_PROP_FOURCC);
	//	//char EXT[] = { (char)(ex & 0XFF),(char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24),0 };
	//	cout << "format: " << ex << endl;
	//	cap.release();
	}

	return 0;
}

