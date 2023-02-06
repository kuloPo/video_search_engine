/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include <filesystem>

#include "io.h"
#include "imgproc.h"

class Test_Reader : public Video_Reader {
public:
	Test_Reader(const std::filesystem::path& filename) : Video_Reader(filename) {
		fps = get_fps(filename);
	}

	virtual void frame_operation() {
		frame_preprocessing(frame);
		noise_frame = make_noise(frame, 1);
		edge_detection(noise_frame, edge_frame);
		//cv::GaussianBlur(noise_frame, edge_frame, cv::Size(7, 7), 3, 0, cv::BORDER_DEFAULT);
		//cv::Mat sobel_x, sobel_y;
		//cv::Mat kernel_x = (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
		//cv::Mat kernel_y = (cv::Mat_<double>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
		//cv::filter2D(edge_frame, sobel_x, -1, kernel_x, cv::Point(-1, -1));
		//cv::filter2D(edge_frame, sobel_y, -1, kernel_y, cv::Point(-1, -1));
		//cv::pow(sobel_x, 2, sobel_x);
		//cv::pow(sobel_y, 2, sobel_y);
		//cv::sqrt((sobel_x + sobel_y), edge_frame);
		cv::imshow("", edge_frame * 5);
		cv::waitKey(fps);
	}
private:
	int fps;
	cv::Mat noise_frame, edge_frame;
};

int main() {
	std::filesystem::path filepath = "../rsrc/video.mp4";
	Test_Reader video_reader(filepath);
	video_reader.run();
	video_reader.print_performance();
	return 0;
}
