// ImageAcquisition.h
#ifndef IMAGE_ACQUISITION_H
#define IMAGE_ACQUISITION_H

#include <iostream>
#include "GenICam/StreamSource.h"
#include "GenICam/System.h"
#include "GenICam/Camera.h"

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

class ImageAcquisition {
public:
	static const unsigned char* acquireImage();
};

#endif // IMAGE_ACQUISITION_H
