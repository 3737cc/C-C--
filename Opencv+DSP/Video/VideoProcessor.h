#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>

class VideoProcessor {
private:
	cv::VideoCapture m_cap;                      // 视频捕获对象
	cv::VideoWriter m_writer;                     // 视频写入对象
	int m_iFrameWidth;                            // 视频宽度
	int m_iFrameHeight;                           // 视频高度
	int m_iFrames;                                // 视频总帧数
	double m_dFps;                                // 视频帧率
	cv::Mat m_frame, m_rgbFrame, m_grayFrame, m_ntscFrame;     // 当前帧
	cv::Mat m_complexI, m_magnitude, m_phase;     // 傅里叶变换相关矩阵
	std::vector<cv::Mat> m_preFrames;             // 处理前的帧
	std::vector<cv::Mat> m_magnitudeFrames;       // 幅度谱帧
	std::vector<cv::Mat> m_phaseFrames;           // 相位谱帧

public:
	VideoProcessor();
	~VideoProcessor();

	void Init();
	bool LoadVideo(const std::string& videoPath);
	bool ExportVideo(const std::string& outputPath, const std::vector<cv::Mat>& processedFrames);
	void ProcessFrame();

	std::vector<cv::Mat> GetImage();
	std::vector<cv::Mat> GetMagnitude();
	std::vector<cv::Mat> GetPhase();
	int GetParameter();
	void SetParameter(int width, int height, int frames, int fps);
	cv::Mat RGBtoYIQ(const cv::Mat& rgbFrame);
	cv::Mat YIQtoRGB(const cv::Mat& yiqFrame);
	cv::Mat YIQtoRGB8Bit(const cv::Mat& yiqFrame); // RGB转换为8位
};

#endif // VIDEOPROCESSOR_H