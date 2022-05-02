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