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
		cv::imshow("", frame);
		cv::waitKey(fps);
	}
private:
	int fps;
};

int main() {
	std::filesystem::path filepath = "../rsrc/video.mp4";
	Test_Reader video_reader(filepath);
	video_reader.run();
	video_reader.print_performance();
	return 0;
}
