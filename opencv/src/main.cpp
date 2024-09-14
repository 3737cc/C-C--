#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
	// 读取图像（使用绝对路径）
	std::string imagePath = "5_06.bmp";
	cv::Mat src = cv::imread(imagePath);

	// 检查图像是否读取成功
	if (src.empty()) {
		std::cerr << "无法读取图像: " << imagePath << std::endl;
		return -1;
	}

	// 创建一个存储旋转后的图像的Mat对象
	cv::Mat dst;

	// 使用OpenCV的旋转函数，将图像旋转90度
	cv::rotate(src, dst, cv::ROTATE_90_CLOCKWISE);

	// 保存旋转后的图像（使用绝对路径）
	std::string outputPath = "output_image.bmp";
	cv::imwrite(outputPath, dst);

	//std::cout << "原图通道数: " << src.channels() << std::endl;
	//std::cout << "旋转后图像通道数: " << dst.channels() << std::endl;

	//std::cout << "原图数据大小: " << src.total() * src.elemSize() << " 字节" << std::endl;
	//std::cout << "旋转后数据大小: " << dst.total() * dst.elemSize() << " 字节" << std::endl;

	//int rowSizeOriginal = src.cols * src.elemSize() + (src.cols * src.elemSize()) % 4;
	//int rowSizeRotated = dst.cols * dst.elemSize() + (dst.cols * dst.elemSize()) % 4;
	//std::cout << "原图每行字节数: " << rowSizeOriginal << std::endl;
	//std::cout << "旋转后每行字节数: " << rowSizeRotated << std::endl;


	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}
