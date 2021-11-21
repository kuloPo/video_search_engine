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

rapidjson::Value write_data(const std::filesystem::path& filename, const std::vector<Key_Frame*>& key_frames, rapidjson::Document::AllocatorType& allocator) {
	std::filesystem::create_directory(index_path / filename);
	std::string ID;
	picosha2::hash256_hex_string(filename.string(), ID);
	int fps = get_fps(video_path / filename);

	int last_frame = 0;

	rapidjson::Value video(rapidjson::kObjectType);
	rapidjson::Value interval(rapidjson::kArrayType);

	for (Key_Frame* key_frame : key_frames) {
		std::string frame_num_str = std::to_string(key_frame->frame_num);
		std::string frame_interval_str = std::to_string(key_frame->frame_num - last_frame);
		interval.PushBack(key_frame->frame_num - last_frame, allocator);
		last_frame = key_frame->frame_num;

		cv::Mat tmp;
		key_frame->first_frame.download(tmp);
		cv::imwrite((index_path / filename / (frame_num_str + "_1.png")).string(), tmp);
		if (key_frame->delta != 0) { // not the first or last frame
			key_frame->second_frame.download(tmp);
			cv::imwrite((index_path / filename / (frame_num_str + "_2.png")).string(), tmp);
		}
	}

	video.AddMember("filename", filename.string(), allocator);
	video.AddMember("ID", ID, allocator);
	video.AddMember("FPS", fps, allocator);
	video.AddMember("interval", interval, allocator);

	return video;
}

int get_fps(const std::filesystem::path& filename) {
	int fps;
	cv::VideoCapture cap(filename.string());
	fps = cvRound(cap.get(cv::CAP_PROP_FPS));
	cap.release();
	return fps;
}