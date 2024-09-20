// CameraControl.h
#ifndef CameraControl_H
#define CameraControl_H

#include <iostream>
#include <vector>
#include "GenICam/Camera.h"
#include "GenICam/System.h"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

class CameraControl {
public:
	bool discoverCameras(TVector<ICameraPtr>& vCameraPtrList);
	bool connectCamera();
	bool disconnectCamera();

private:
	// 相机系统对象
	CSystem& systemObj = CSystem::getInstance();
};

#endif
