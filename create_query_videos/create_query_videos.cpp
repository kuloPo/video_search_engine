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

#include <iostream>
#include <filesystem>
#include <queue>
#include <thread>
#include <fstream>
#include <mutex>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#else
#include <opencv2/videoio.hpp>
#endif

#include "io.h"

using std::cout;
using std::endl;

std::filesystem::path output_dir = "../output";

std::queue<std::filesystem::path> working_queue;
std::mutex queue_mutex, f_mutex;
std::ofstream fd;

void add_flip(cv::Mat& frame) {
    cv::flip(frame, frame, 1);
}

void add_blur(cv::Mat& frame) {
    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 1, 1, cv::BORDER_DEFAULT);
}

void add_resize(cv::Mat& frame) {
    cv::Size raw_size = frame.size();
    cv::resize(frame, frame, raw_size / 2);
    cv::resize(frame, frame, raw_size);
}

void add_noise(cv::Mat& frame) {
    cv::Mat noise(frame.size(), frame.type());
    cv::randn(noise, 30, 15);
    frame += noise;
}

void add_brightness(cv::Mat& frame) {
    frame += cv::Scalar(50, 50, 50);
}

void add_crop(cv::Mat& frame) {
    int h = frame.rows;
    int w = frame.cols;
    int cropped_amount = h * 0.1;
    cv::Mat cropped = frame(cv::Range(cropped_amount, h - cropped_amount), cv::Range(0, w));
    frame = cv::Mat(frame.size(), frame.type(), cv::Scalar(0));
    cropped.copyTo(frame(cv::Rect(0, cropped_amount, cropped.cols, cropped.rows)));
}

void add_logo(cv::Mat& frame) {
    cv::putText(frame,
        "LOGO",
        cv::Point(30, 60),
        cv::FONT_HERSHEY_DUPLEX,
        2,
        CV_RGB(255, 255, 255),
        10);
}

void add_perspective(cv::Mat& frame) {
    cv::Mat srcTri = (cv::Mat_<float>(4, 2) << 56, 65, 368, 52, 28, 387, 389, 390);
    cv::Mat dstTri = (cv::Mat_<float>(4, 2) << 0, 0, 300, 0, 0, 300, 300, 300);
    cv::Mat M = cv::getPerspectiveTransform(srcTri, dstTri);
    cv::warpPerspective(frame, frame, M, frame.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
}

std::vector<void (*)(cv::Mat& frame)> filters{
    add_flip, 
    add_blur, 
    add_resize, 
    add_noise,
    add_brightness,
    add_crop,
    add_logo,
    add_perspective,
};

std::vector<std::string> filters_name{
    "flip",
    "blur",
    "resize",
    "noise",
    "brightness",
    "crop",
    "logo",
    "perspective",
};

void apply_filters(cv::Mat& frame, const std::vector<int>& filter_ID) {
    for (int i : filter_ID) {
        (*filters[i])(frame);
    }
}

std::vector<int> choose_filter() {
    std::vector<int> filter_ID;
    for (int i = 0; i < filters.size(); i++) {
        if ((float)rand() / RAND_MAX < 0.3) {
            filter_ID.push_back(i);
        }
    }
    return filter_ID;
}

void create_filter(const std::filesystem::path& video_path) {
    std::filesystem::path output_path = output_dir / video_path.filename();
    output_path.replace_extension("avi");
    std::vector<int> filter_ID = choose_filter();
    f_mutex.lock();
    cout << video_path.filename().string() << " ";
    fd << video_path.filename().string() << " ";
    for (int ID : filter_ID) {
        cout << filters_name[ID] << " ";
        fd << filters_name[ID] << " ";
    }
    cout << endl;
    fd << endl;
    f_mutex.unlock();

    cv::Mat frame;
#ifdef HAVE_OPENCV_CUDACODEC
    cv::Ptr<cv::cudacodec::VideoReader> video_reader = cv::cudacodec::createVideoReader(video_path.string());
    cv::cuda::GpuMat gpu_frame;
    video_reader->nextFrame(gpu_frame);
    gpu_frame.download(frame);
#else
    cv::VideoCapture video_reader(video_path.string());
    video_reader >> frame;
#endif
    cv::VideoWriter video_writer(output_path.string(), cv::VideoWriter::fourcc('M', 'P', 'E', 'G'), get_fps(video_path), frame.size());
    while (true) {
#ifdef HAVE_OPENCV_CUDACODEC
        if (!video_reader->nextFrame(gpu_frame))
            break;
        gpu_frame.download(frame);
#else
        video_reader >> frame;
        if (frame.empty())
            break;
#endif
        apply_filters(frame, filter_ID);
        video_writer << frame;
    }
    video_reader.release();
    video_writer.release();
}

void thread_invoker(int deviceID) {
#ifdef HAVE_OPENCV_CUDACODEC
    cv::cuda::setDevice(deviceID);
#endif
    while (working_queue.empty() == false) {
        queue_mutex.lock();
        std::filesystem::path filename = working_queue.front();
        working_queue.pop();
        queue_mutex.unlock();
        create_filter(filename);
    }
}

int main() {
    srand(time(0));
    cv::setNumThreads(0);
    read_config("../rsrc/config.ini");
    std::filesystem::path log_path = "./ground_truth_" + std::to_string(time(0)) + ".log";
    fd.open(log_path, std::ios::in | std::ios::trunc);

    std::filesystem::path video_path = "D:\\datasets\\MUSCLE_VCD_2007";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(video_path)) {
        working_queue.push(entry.path());
    }

    std::vector<std::thread> thread_list;
    for (int i = 0; i < thread_num; i++) {
        std::thread t(thread_invoker, i);
        thread_list.push_back(std::move(t));
    }

    for (auto iter = thread_list.begin(); iter != thread_list.end(); iter++) {
        iter->join();
    }
    fd.close();
}