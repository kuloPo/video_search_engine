/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.

 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include "debug.h"

void plot(
    const std::string& name, 
    const cv::Mat& data, 
    const double max_h, 
    const double max_w,
    const std::string& text)
{
    cv::Mat src, plot_result;
    data.convertTo(src, CV_64FC1);

    cv::Ptr<cv::plot::Plot2d> plot = cv::plot::Plot2d::create(src);
    plot->setShowGrid(false);
    plot->setShowText(false);
    plot->setInvertOrientation(true);
    plot->setMaxY(max_h);
    plot->setMaxX(max_w);
    plot->setMinX(0);
    plot->setMinY(0);
    plot->render(plot_result);

    cv::putText(plot_result,
        text,
        cv::Point(10, 50),
        cv::FONT_HERSHEY_DUPLEX,
        1.0,
        CV_RGB(255, 255, 255),
        1);

    imshow(name.c_str(), plot_result);
}

void plot_delta(double new_data, int frame_num, double max_h) {
    static std::vector<double> data;
    if (data.size() >= 500) {
        data.clear();
    }
    data.push_back(new_data);
    cv::Mat data_mat(1, data.size(), CV_64F, data.data());
    plot("plot", data_mat, max_h, 500, std::to_string(frame_num));
}