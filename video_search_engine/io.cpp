#include "io.h"

void show_image(const std::vector<Key_Frame*>& key_frames) {
	for (Key_Frame* key_frame : key_frames) {
		printf("%d %.2f\n", key_frame->frame_num, key_frame->delta);
		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imshow("", tmp);
		cv::waitKey();
		key_frame->second_frame.download(tmp);
		cv::imshow("", tmp);
		cv::waitKey();
	}
}

void write_data(const std::filesystem::path& filename, const std::vector<Key_Frame*>& key_frames) {
	std::filesystem::create_directory(key_frame_path / filename);
	std::ofstream fs;
	fs.open(csv_path, std::ofstream::out | std::ofstream::app);
	fs << filename;
	for (Key_Frame* key_frame : key_frames) {
		std::string frame_num_str = std::to_string(key_frame->frame_num);
		fs << "," + frame_num_str;

		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imwrite((key_frame_path / filename / (frame_num_str + "_1.png")).string(), tmp);
		key_frame->second_frame.download(tmp);
		cv::imwrite((key_frame_path / filename / (frame_num_str + "_2.png")).string(), tmp);
	}
	fs << std::endl;
	fs.close();
}