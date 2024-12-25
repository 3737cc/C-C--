#ifndef IMFILTER_H
#define IMFILTER_H

#include <opencv2/opencv.hpp>

// 声明循环卷积函数
cv::Mat customConvolution(const cv::Mat& src, const cv::Mat& kernel, const std::string& mode);

cv::Mat conv2_valid(const cv::Mat& image, const cv::Mat& kernel);

#endif // IMFILTER_H
