#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
	// ��ȡͼ��ʹ�þ���·����
	std::string imagePath = "5_06.bmp";
	cv::Mat src = cv::imread(imagePath);

	// ���ͼ���Ƿ��ȡ�ɹ�
	if (src.empty()) {
		std::cerr << "�޷���ȡͼ��: " << imagePath << std::endl;
		return -1;
	}

	// ����һ���洢��ת���ͼ���Mat����
	cv::Mat dst;

	// ʹ��OpenCV����ת��������ͼ����ת90��
	cv::rotate(src, dst, cv::ROTATE_90_CLOCKWISE);

	// ������ת���ͼ��ʹ�þ���·����
	std::string outputPath = "output_image.bmp";
	cv::imwrite(outputPath, dst);

	//std::cout << "ԭͼͨ����: " << src.channels() << std::endl;
	//std::cout << "��ת��ͼ��ͨ����: " << dst.channels() << std::endl;

	//std::cout << "ԭͼ���ݴ�С: " << src.total() * src.elemSize() << " �ֽ�" << std::endl;
	//std::cout << "��ת�����ݴ�С: " << dst.total() * dst.elemSize() << " �ֽ�" << std::endl;

	//int rowSizeOriginal = src.cols * src.elemSize() + (src.cols * src.elemSize()) % 4;
	//int rowSizeRotated = dst.cols * dst.elemSize() + (dst.cols * dst.elemSize()) % 4;
	//std::cout << "ԭͼÿ���ֽ���: " << rowSizeOriginal << std::endl;
	//std::cout << "��ת��ÿ���ֽ���: " << rowSizeRotated << std::endl;


	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}
