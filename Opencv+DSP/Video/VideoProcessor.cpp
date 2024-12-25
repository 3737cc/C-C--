#include "VideoProcessor.h"

VideoProcessor::VideoProcessor() {
	Init();
}

VideoProcessor::~VideoProcessor() {
	if (m_cap.isOpened()) {
		m_cap.release();
	}
	if (m_writer.isOpened()) {
		m_writer.release();
	}
}

void VideoProcessor::Init() {
	m_iFrameWidth = 0;
	m_iFrameHeight = 0;
	m_iFrames = 0;
	m_dFps = 0;
}

bool VideoProcessor::LoadVideo(const std::string& videoPath) {
	m_cap.open(videoPath);
	if (!m_cap.isOpened()) {
		std::cerr << "Error: Could not open video file " << videoPath << std::endl;
		return false;
	}

	m_iFrameWidth = static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_WIDTH));
	m_iFrameHeight = static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_HEIGHT));
	m_iFrames = static_cast<int>(m_cap.get(cv::CAP_PROP_FRAME_COUNT));
	m_dFps = m_cap.get(cv::CAP_PROP_FPS);

	std::cout << "视频宽度*高度=" << m_iFrameWidth << "*" << m_iFrameHeight << std::endl;
	std::cout << "视频的总帧数：" << m_iFrames << std::endl;
	std::cout << "视频的帧率：" << m_dFps << std::endl;

	int i = 0;
	while (true) {
		m_cap >> m_frame;
		if (m_frame.empty()) {
			break;
		}
		cv::cvtColor(m_frame, m_rgbFrame, cv::COLOR_BGR2RGB);
		//m_ntscFrame = RGBtoYIQ(m_rgbFrame);
		//m_rgbFrame = YIQtoRGB(m_ntscFrame);
		//m_rgbFrame = YIQtoRGB8Bit(m_ntscFrame);
		m_preFrames.push_back(m_rgbFrame.clone());
		i++;
	}
	return true;
}

bool VideoProcessor::ExportVideo(const std::string& outputPath, const std::vector<cv::Mat>& processedFrames) {
	if (processedFrames.empty()) {
		std::cerr << "Error: No processed frames to export" << std::endl;
		return false;
	}

	m_writer.open(outputPath,
		cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
		m_dFps,
		cv::Size(processedFrames[0].cols, processedFrames[0].rows));

	if (!m_writer.isOpened()) {
		std::cerr << "Error: Could not open output video file " << outputPath << std::endl;
		return false;
	}

	for (const auto& frame : processedFrames) {
		if (frame.empty()) {
			std::cerr << "Warning: Encountered empty frame, skipping..." << std::endl;
			continue;
		}
		m_writer.write(frame);
	}

	m_writer.release();
	return true;
}

void VideoProcessor::ProcessFrame() {
	cv::cvtColor(m_frame, m_grayFrame, cv::COLOR_BGR2GRAY);
	m_grayFrame.convertTo(m_grayFrame, CV_32F);
	cv::Mat planes[2];
	planes[0] = m_grayFrame;
	planes[1] = cv::Mat::zeros(m_grayFrame.size(), CV_32F);
	cv::merge(planes, 2, m_complexI);
	cv::dft(m_complexI, m_complexI);
	cv::split(m_complexI, planes);
	cv::magnitude(planes[0], planes[1], m_magnitude);
	cv::log(m_magnitude + 1, m_magnitude);
	cv::normalize(m_magnitude, m_magnitude, 0, 1, cv::NORM_MINMAX);
	cv::phase(planes[0], planes[1], m_phase);
}

std::vector<cv::Mat> VideoProcessor::GetImage() {
	return m_preFrames;
}

std::vector<cv::Mat> VideoProcessor::GetMagnitude() {
	return m_magnitudeFrames;
}

std::vector<cv::Mat> VideoProcessor::GetPhase() {
	return m_phaseFrames;
}

int VideoProcessor::GetParameter() {
	return m_iFrameWidth, m_iFrameHeight, m_iFrames, m_dFps;
}

void VideoProcessor::SetParameter(int width, int height, int frames, int fps) {
	m_iFrameWidth = width;
	m_iFrameHeight = height;
	m_iFrames = frames;
	m_dFps = fps;
}

cv::Mat VideoProcessor::RGBtoYIQ(const cv::Mat& rgbFrame) {
	cv::Mat rgbFloat;
	if (rgbFrame.type() != CV_64FC3) {
		rgbFrame.convertTo(rgbFloat, CV_64F, 1.0 / 255.0);
	}
	else {
		rgbFloat = rgbFrame.clone();
	}

	cv::Mat yiqMatrix = (cv::Mat_<double>(3, 3) <<
		0.299, 0.587, 0.114,    // Y
		0.596, -0.274, -0.322,  // I
		0.211, -0.523, 0.312    // Q
		);

	cv::Mat yiqFrame(rgbFloat.size(), CV_64FC3);
	for (int i = 0; i < rgbFloat.rows; i++) {
		for (int j = 0; j < rgbFloat.cols; j++) {
			cv::Vec3d rgb = rgbFloat.at<cv::Vec3d>(i, j);
			cv::Mat rgbMat = (cv::Mat_<double>(3, 1) << rgb[0], rgb[1], rgb[2]);
			cv::Mat yiq = yiqMatrix * rgbMat;

			yiqFrame.at<cv::Vec3d>(i, j) = cv::Vec3d(yiq.at<double>(0),
				yiq.at<double>(1),
				yiq.at<double>(2));
		}
	}
	return yiqFrame;
}

cv::Mat VideoProcessor::YIQtoRGB(const cv::Mat& yiqFrame) {
	CV_Assert(yiqFrame.type() == CV_64FC3);

	cv::Mat rgbMatrix = (cv::Mat_<double>(3, 3) <<
		1.000, 0.956, 0.621,     // R
		1.000, -0.272, -0.647,   // G
		1.000, -1.105, 1.702     // B
		);

	cv::Mat rgbFrame(yiqFrame.size(), CV_64FC3);

	for (int i = 0; i < yiqFrame.rows; i++) {
		for (int j = 0; j < yiqFrame.cols; j++) {
			cv::Vec3d yiq = yiqFrame.at<cv::Vec3d>(i, j);
			cv::Mat yiqMat = (cv::Mat_<double>(3, 1) << yiq[0], yiq[1], yiq[2]);

			cv::Mat rgb = rgbMatrix * yiqMat;

			rgb.at<double>(0) = std::min(std::max(rgb.at<double>(0), 0.0), 1.0);
			rgb.at<double>(1) = std::min(std::max(rgb.at<double>(1), 0.0), 1.0);
			rgb.at<double>(2) = std::min(std::max(rgb.at<double>(2), 0.0), 1.0);

			rgbFrame.at<cv::Vec3d>(i, j) = cv::Vec3d(rgb.at<double>(0),
				rgb.at<double>(1),
				rgb.at<double>(2));
		}
	}

	return rgbFrame;
}

cv::Mat VideoProcessor::YIQtoRGB8Bit(const cv::Mat& yiqFrame) {
	cv::Mat rgbFloat = YIQtoRGB(yiqFrame);
	cv::Mat rgb8Bit;
	rgbFloat.convertTo(rgb8Bit, CV_8UC3, 255.0);
	return rgb8Bit;
}