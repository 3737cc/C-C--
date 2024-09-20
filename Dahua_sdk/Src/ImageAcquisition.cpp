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
	// ����������
	IStreamSourcePtr streamPtr = systemObj.createStreamSource(camera);
	streamPtr->startGrabbing();
	CFrame frame_image;

	// ��ȡͼ������
	const void* rawData = frame_image.getImage();
	const unsigned char* data = static_cast<const unsigned char*>(rawData);

	// ����ͼ������
	return data;
}
