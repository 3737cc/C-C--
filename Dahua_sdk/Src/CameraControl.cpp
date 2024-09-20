// CameraControl.cpp
#include "CameraControl.h"

bool CameraControl::discoverCameras(TVector<ICameraPtr>& vCameraPtrList) {
	if (systemObj.discovery(vCameraPtrList)) {
		if (vCameraPtrList.empty()) {
			std::cout << "Discovery succeeded, but no cameras were found." << std::endl;
			return false;
		}
		std::cout << "Successfully discovered " << vCameraPtrList.size() << " camera(s)." << std::endl;
		return true;
	}
	else {
		std::cout << "Failed to discover any cameras." << std::endl;
		return false;
	}
}

bool CameraControl::connectCamera() {
	TVector<ICameraPtr> vCameraPtrList;
	if (!discoverCameras(vCameraPtrList)) {
		return false;
	}

	ICameraPtr camera = vCameraPtrList[0];
	try {
		camera->connect();
		if (camera->isConnected()) {
			std::cout << "Camera successfully connected." << std::endl;
			return true;
		}
		else {
			std::cout << "Failed to connect the camera." << std::endl;
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error connecting to the camera: " << e.what() << std::endl;
		return false;
	}
}

bool CameraControl::disconnectCamera() {
	TVector<ICameraPtr> vCameraPtrList;
	if (!discoverCameras(vCameraPtrList)) {
		return false;
	}

	ICameraPtr camera = vCameraPtrList[0];
	try {
		camera->disConnect();
		if (!camera->isConnected()) {
			std::cout << "Camera successfully disconnected." << std::endl;
			return true;
		}
		else {
			std::cout << "Failed to disconnect the camera." << std::endl;
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error disconnecting the camera: " << e.what() << std::endl;
		return false;
	}
}
