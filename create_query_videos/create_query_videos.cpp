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
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#else
#include <opencv2/videoio.hpp>
#endif

using std::cout;
using std::endl;

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
    cv::randn(noise, 20, 10);
    frame += noise;
}

void add_brightness(cv::Mat& frame) {
    frame += cv::Scalar(50, 50, 50);
}

void add_crop(cv::Mat& frame) {
    int h = frame.rows;
    int w = frame.cols;
    int cropped_amount = h * 0.1;
    frame = frame(cv::Range(cropped_amount, h - cropped_amount), cv::Range(0, w));
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

int main() {
    //std::queue<std::filesystem::path> working_queue;
    //std::filesystem::path video_path = "D:\\datasets\\MUSCLE_VCD_2007";
    //for (const auto& entry : std::filesystem::directory_iterator(video_path)) {
    //  working_queue.push(entry.path().filename());
    //}
    //while (working_queue.empty() == false) {
    //  std::filesystem::path filename = working_queue.front();
    //  working_queue.pop();
    //  cout << filename << endl;
    //}
    std::filesystem::path filepath = "../rsrc/video.mp4";
    cv::Mat frame;
#ifdef HAVE_OPENCV_CUDACODEC
    cv::Ptr<cv::cudacodec::VideoReader> video_reader = cv::cudacodec::createVideoReader(filepath.string());
    cv::cuda::GpuMat gpu_frame;
    video_reader->nextFrame(gpu_frame);
    gpu_frame.download(frame);
#else
    cv::VideoCapture video_reader(filepath.string());
    cv::Mat frame;
    video_reader >> frame;
#endif
    cv::VideoWriter video_writer("outcpp.avi", cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), 25, frame.size());
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

        //add_flip(frame);
        //add_blur(frame);
        //add_resize(frame);
        //add_noise(frame);
        //add_brightness(frame);
        //add_crop(frame);
        //add_logo(frame);

        //cv::imshow("", frame);
        //cv::waitKey(25);
        video_writer.write(frame);
    }
    video_reader.release();
    video_writer.release();
}