#include "imfilter.h"
#include <opencv2/opencv.hpp>
#include <iostream>

// 自定义卷积函数，支持不同模式
cv::Mat customConvolution(const cv::Mat& src, const cv::Mat& kernel, const std::string& mode) {
    if (mode == "circular") {
        int pad = kernel.rows / 2;
        cv::Mat paddedImage;

        // 使用循环边界填充
        cv::copyMakeBorder(src, paddedImage, pad, pad, pad, pad, cv::BORDER_WRAP);

        // 创建输出图像，确保使用与输入相同的数据类型
        cv::Mat dst = cv::Mat::zeros(src.size(), src.type());

        // 手动卷积
        for (int y = 0; y < src.rows; y++) {
            for (int x = 0; x < src.cols; x++) {
                float sum = 0.0f;

                // 对卷积核的每个元素进行卷积
                for (int ky = 0; ky < kernel.rows; ky++) {
                    for (int kx = 0; kx < kernel.cols; kx++) {
                        // 计算对应的图像坐标（使用循环边界）
                        int imgY = (y + ky) % paddedImage.rows;
                        int imgX = (x + kx) % paddedImage.cols;

                        // 累加卷积结果
                        sum += paddedImage.at<float>(imgY, imgX) * kernel.at<float>(ky, kx);
                    }
                }

                // 存储卷积结果
                dst.at<float>(y, x) = sum;
            }
        }

        return dst;
    }
    else {
        std::cerr << "Unsupported mode: " << mode << std::endl;
        return cv::Mat();
    }
}

cv::Mat conv2_valid(const cv::Mat& image, const cv::Mat& kernel) {
    cv::Mat result;

    // 执行二维卷积
    cv::filter2D(image, result, -1, kernel);

    // 计算 valid 区域的大小
    int borderX = kernel.cols / 2;
    int borderY = kernel.rows / 2;

    // 裁剪 valid 区域，去除边界部分
    result = result(cv::Rect(borderX, borderY, image.cols - kernel.cols + 1, image.rows - kernel.rows + 1));
    // 在 result 中将非零元素加上 0.2
    result.forEach<float>([](float& pixel, const int* position) -> void {
        if (pixel != 0) {
            pixel += 0.2f;
        }
        });
    return result;
}
