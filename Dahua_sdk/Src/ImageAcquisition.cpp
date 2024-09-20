// ImageAcquisition.cpp
#include "ImageAcquisition.h"

const unsigned char* ImageAcquisition::acquireImage() {
	CSystem& systemObj = CSystem::getInstance();
	TVector<ICameraPtr> vCameraPtrList;
	bool isDiscoverySuccess = systemObj.discovery(vCameraPtrList);

	if (vCameraPtrList.empty()) {
		return nullptr;
	}

	ICameraPtr camera = vCameraPtrList[0];
	// 创建流对象
	IStreamSourcePtr streamPtr = systemObj.createStreamSource(camera);
	streamPtr->startGrabbing();
	CFrame frame_image;

	// 获取图像数据
	const void* rawData = frame_image.getImage();
	const unsigned char* data = static_cast<const unsigned char*>(rawData);

	// 返回图像数据
	return data;
}
