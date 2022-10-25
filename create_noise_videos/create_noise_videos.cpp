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
#include <mutex>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#ifdef HAVE_OPENCV_CUDACODEC
#include <opencv2/cudacodec.hpp>
#endif

#include "io.h"

using std::cout;
using std::endl;

std::filesystem::path output_dir = "D:\\datasets\\NIST_TREC_VIDEO_BG_100";

std::queue<std::filesystem::path> working_queue;
std::mutex queue_mutex;

void create_video(const std::filesystem::path& video_path) {
    std::filesystem::path output_path = output_dir / video_path.filename();
    output_path.replace_extension("mp4");

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
    cv::VideoWriter video_writer(output_path.string(), cv::VideoWriter::fourcc('H', '2', '6', '4'), get_fps(video_path), frame.size());
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
        
        video_writer << frame;
    }
    video_reader.release();
    video_writer.release();
}

void thread_invoker(int deviceID) {
#ifdef HAVE_OPENCV_CUDACODEC
    cv::cuda::setDevice(deviceID);
#endif
    while (true) {
        queue_mutex.lock();
        if (working_queue.empty()) {
            queue_mutex.unlock();
            break;
        }
        std::filesystem::path filename = working_queue.front();
        working_queue.pop();
        queue_mutex.unlock();
        create_video(filename);
    }
}

int main() {
    srand(time(0));
    cv::setNumThreads(0);
    read_config();

    for (const auto& entry : std::filesystem::recursive_directory_iterator(MUSCLE_VCD_2007)) {
        if (std::filesystem::is_regular_file(entry.path())) {
            working_queue.push(entry.path());
        }
    }

    std::vector<std::thread> thread_list;
    for (int i = 0; i < thread_num; i++) {
        std::thread t(thread_invoker, i);
        thread_list.push_back(std::move(t));
    }

    for (auto iter = thread_list.begin(); iter != thread_list.end(); iter++) {
        iter->join();
    }
}