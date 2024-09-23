#include "CammerWidget.h"
#include "ui_cammerwidget.h"
#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"
#include <qpainter>
#include <QMouseEvent>
#include <QWheelEvent>


/*#define DEFAULT_SHOW_RATE (60)*/ // 默认显示帧率 | defult display frequency
#define DEFAULT_ERROR_STRING ("N/A") 
#define MAX_FRAME_STAT_NUM (50) 
#define MIN_LEFT_LIST_NUM (2) 
#define MAX_STATISTIC_INTERVAL (5000000000) // List的更新时与最新一帧的时间最大间隔 |  The maximum time interval between the update of list and the latest frame

int showRate = 30;
int qwidth = 1920, qheight = 1024;
int acquisition = 30;

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

CammerWidget::CammerWidget(QWidget* parent) :
	QWidget(parent)
	, ui(new Ui::CammerWidget)
	, thdDisplayThread(CThreadLite::ThreadProc(&CammerWidget::DisplayThreadProc, this), "Display")
	, nDisplayInterval(0)
	, nFirstFrameTime(0)
	, nLastFrameTime(0)
	, bNeedUpdate(true)
	, nTotalFrameCount(0),
	zoomFactor(1.0f),
	imageSet(false)
{
	ui->setupUi(this);

	qRegisterMetaType<uint64_t>("uint64_t");
	connect(this, SIGNAL(signalShowImage(uint8_t*, int, int, uint64_t)), this, SLOT(ShowImage(uint8_t*, int, int, uint64_t)));

	setDisplayFPS(showRate);
	elapsedTimer.start();

	// 启动显示线程
	// start display thread
	if (!thdDisplayThread.isThreadOver())
	{
		thdDisplayThread.destroyThread();
	}
	thdDisplayThread.createThread();
}

CammerWidget::~CammerWidget()
{
	// 关闭显示线程
	// close display thread
	if (!thdDisplayThread.isThreadOver())
	{
		thdDisplayThread.destroyThread();
	}

	delete ui;
}

// 取流回调函数
// get frame callback function 
void CammerWidget::FrameCallback(const CFrame& frame)
{
	CFrameInfo frameInfo;
	frameInfo.nWidth = frame.getImageWidth();
	frameInfo.nHeight = frame.getImageHeight();
	frameInfo.nBufferSize = frame.getImageSize();
	frameInfo.nPaddingX = frame.getImagePadddingX();
	frameInfo.nPaddingY = frame.getImagePadddingY();
	frameInfo.ePixelType = frame.getImagePixelFormat();
	frameInfo.pImageBuf = (BYTE*)malloc(sizeof(BYTE) * frameInfo.nBufferSize);
	frameInfo.nTimeStamp = frame.getImageTimeStamp();

	// 内存申请失败，直接返回
	// memory application failed, return directly
	if (frameInfo.pImageBuf != NULL)
	{
		memcpy(frameInfo.pImageBuf, frame.getImage(), frame.getImageSize());

		if (qDisplayFrameQueue.size() > 16)
		{
			CFrameInfo frameOld;
			qDisplayFrameQueue.get(frameOld);
			free(frameOld.pImageBuf);
		}

		qDisplayFrameQueue.push_back(frameInfo);
	}

	recvNewFrame(frame);
}

//单帧处理
void CammerWidget::FrameSingle(const CFrame& frame)
{
	CFrameInfo frameInfo;
	frameInfo.nWidth = frame.getImageWidth();
	frameInfo.nHeight = frame.getImageHeight();
	frameInfo.nBufferSize = frame.getImageSize();
	frameInfo.nPaddingX = frame.getImagePadddingX();
	frameInfo.nPaddingY = frame.getImagePadddingY();
	frameInfo.ePixelType = frame.getImagePixelFormat();
	frameInfo.pImageBuf = (BYTE*)malloc(sizeof(BYTE) * frameInfo.nBufferSize);
	frameInfo.nTimeStamp = frame.getImageTimeStamp();

	// 内存申请失败，直接返回
	// memory application failed, return directly
	if (frameInfo.pImageBuf != NULL)
	{
		memcpy(frameInfo.pImageBuf, frame.getImage(), frame.getImageSize());

		// 只处理第一帧
		if (qDisplayFrameQueue.size() == 0)  // 使用 size() 检查是否为空
		{
			qDisplayFrameQueue.push_back(frameInfo);
			// 停止抓取
			if (pStreamSource)
			{
				pStreamSource->stopGrabbing();
			}
		}
		else
		{
			free(frameInfo.pImageBuf); // 如果有多余的帧数据，释放内存
		}
	}

	recvNewFrame(frame);
}

// 设置曝光
// set exposeTime
bool CammerWidget::SetExposeTime(double dExposureTime)
{
	if (NULL == pCamera)
	{
		printf("Set ExposureTime fail. No camera or camera is not connected.\n");
		return false;
	}

	CDoubleNode nodeExposureTime(pCamera, "ExposureTime");

	if (false == nodeExposureTime.isValid())
	{
		printf("get ExposureTime node fail.\n");
		return false;
	}

	if (false == nodeExposureTime.isAvailable())
	{
		printf("ExposureTime is not available.\n");
		return false;
	}

	if (false == nodeExposureTime.setValue(dExposureTime))
	{
		printf("set ExposureTime value = %f fail.\n", dExposureTime);
		return false;
	}

	return true;
}

// 设置增益
// set gain
bool CammerWidget::SetAdjustPlus(double dGainRaw)
{
	if (NULL == pCamera)
	{
		printf("Set GainRaw fail. No camera or camera is not connected.\n");
		return false;
	}

	CDoubleNode nodeGainRaw(pCamera, "GainRaw");

	if (false == nodeGainRaw.isValid())
	{
		printf("get GainRaw node fail.\n");
		return false;
	}

	if (false == nodeGainRaw.isAvailable())
	{
		printf("GainRaw is not available.\n");
		return false;
	}

	if (false == nodeGainRaw.setValue(dGainRaw))
	{
		printf("set GainRaw value = %f fail.\n", dGainRaw);
		return false;
	}

	return true;
}

// 打开相机
// open camera
bool CammerWidget::CameraOpen(void)
{
	if (NULL == pCamera)
	{
		printf("connect camera fail. No camera.\n");
		return false;
	}

	if (true == pCamera->isConnected())
	{
		printf("camera is already connected.\n");
		return true;
	}

	if (false == pCamera->connect())
	{
		printf("connect camera fail.\n");
		return false;
	}

	return true;
}

// 关闭相机
// close camera
bool CammerWidget::CameraClose(void)
{
	if (NULL == pCamera)
	{
		printf("disconnect camera fail. No camera.\n");
		return false;
	}

	if (false == pCamera->isConnected())
	{
		printf("camera is already disconnected.\n");
		return true;
	}

	if (false == pCamera->disConnect())
	{
		printf("disconnect camera fail.\n");
		return false;
	}

	return true;
}

// 开始采集
// start grabbing
bool CammerWidget::CameraStart()
{
	if (NULL == pStreamSource)
	{
		pStreamSource = CSystem::getInstance().createStreamSource(pCamera);
	}

	if (NULL == pStreamSource)
	{
		return false;
	}

	if (pStreamSource->isGrabbing())
	{
		return true;
	}

	bool bRet = pStreamSource->attachGrabbing(IStreamSource::Proc(&CammerWidget::FrameCallback, this));
	if (!bRet)
	{
		return false;
	}

	if (!pStreamSource->startGrabbing())
	{
		return false;
	}

	return true;
}

//单张采集
bool CammerWidget::CaptureSingleImage()
{
	if (NULL == pStreamSource)
	{
		pStreamSource = CSystem::getInstance().createStreamSource(pCamera);
	}

	if (NULL == pStreamSource)
	{
		return false;
	}

	if (pStreamSource->isGrabbing())
	{
		return true;
	}

	bool bRet = pStreamSource->attachGrabbing(IStreamSource::Proc(&CammerWidget::FrameSingle, this));
	if (!bRet)
	{
		return false;
	}

	if (!pStreamSource->startGrabbing())
	{
		return false;
	}

	return true;
}


// 停止采集
// stop grabbing
bool CammerWidget::CameraStop()
{
	if (pStreamSource != NULL)
	{
		pStreamSource->detachGrabbing(IStreamSource::Proc(&CammerWidget::FrameCallback, this));

		pStreamSource->stopGrabbing();
		pStreamSource.reset();
	}

	// 清空显示队列 
	// clear display queue
	qDisplayFrameQueue.clear();

	return true;
}

// 切换采集方式、触发方式 （连续采集、外部触发、软件触发）
// Switch acquisition mode and triggering mode (continuous acquisition, external triggering and software triggering)
bool CammerWidget::CameraChangeTrig(ETrigType trigType)
{
	if (NULL == pCamera)
	{
		printf("Change Trig fail. No camera or camera is not connected.\n");
		return false;
	}

	if (trigContinous == trigType)
	{
		// 设置触发模式
		// set trigger mode
		CEnumNode nodeTriggerMode(pCamera, "TriggerMode");
		if (false == nodeTriggerMode.isValid())
		{
			printf("get TriggerMode node fail.\n");
			return false;
		}
		if (false == nodeTriggerMode.setValueBySymbol("Off"))
		{
			printf("set TriggerMode value = Off fail.\n");
			return false;
		}
	}
	else if (trigSoftware == trigType)
	{
		// 设置触发器
		// set trigger
		CEnumNode nodeTriggerSelector(pCamera, "TriggerSelector");
		if (false == nodeTriggerSelector.isValid())
		{
			printf("get TriggerSelector node fail.\n");
			return false;
		}
		if (false == nodeTriggerSelector.setValueBySymbol("FrameStart"))
		{
			printf("set TriggerSelector value = FrameStart fail.\n");
			return false;
		}

		// 设置触发模式
		// set trigger mode
		CEnumNode nodeTriggerMode(pCamera, "TriggerMode");
		if (false == nodeTriggerMode.isValid())
		{
			printf("get TriggerMode node fail.\n");
			return false;
		}
		if (false == nodeTriggerMode.setValueBySymbol("On"))
		{
			printf("set TriggerMode value = On fail.\n");
			return false;
		}

		// 设置触发源为软触发
		// set triggerSource as software trigger
		CEnumNode nodeTriggerSource(pCamera, "TriggerSource");
		if (false == nodeTriggerSource.isValid())
		{
			printf("get TriggerSource node fail.\n");
			return false;
		}
		if (false == nodeTriggerSource.setValueBySymbol("Software"))
		{
			printf("set TriggerSource value = Software fail.\n");
			return false;
		}
	}
	else if (trigLine == trigType)
	{
		// 设置触发器
		// set trigger
		CEnumNode nodeTriggerSelector(pCamera, "TriggerSelector");
		if (false == nodeTriggerSelector.isValid())
		{
			printf("get TriggerSelector node fail.\n");
			return false;
		}
		if (false == nodeTriggerSelector.setValueBySymbol("FrameStart"))
		{
			printf("set TriggerSelector value = FrameStart fail.\n");
			return false;
		}

		// 设置触发模式
		// set trigger mode
		CEnumNode nodeTriggerMode(pCamera, "TriggerMode");
		if (false == nodeTriggerMode.isValid())
		{
			printf("get TriggerMode node fail.\n");
			return false;
		}
		if (false == nodeTriggerMode.setValueBySymbol("On"))
		{
			printf("set TriggerMode value = On fail.\n");
			return false;
		}

		// 设置触发源为Line1触发
		// set trigggerSource as Line1 trigger 
		CEnumNode nodeTriggerSource(pCamera, "TriggerSource");
		if (false == nodeTriggerSource.isValid())
		{
			printf("get TriggerSource node fail.\n");
			return false;
		}
		if (false == nodeTriggerSource.setValueBySymbol("Line1"))
		{
			printf("set TriggerSource value = Line1 fail.\n");
			return false;
		}
	}

	return true;
}

// 执行一次软触发
// execute one software trigger
bool CammerWidget::ExecuteSoftTrig(void)
{
	if (NULL == pCamera)
	{
		printf("Set GainRaw fail. No camera or camera is not connected.\n");
		return false;
	}

	CCmdNode nodeTriggerSoftware(pCamera, "TriggerSoftware");
	if (false == nodeTriggerSoftware.isValid())
	{
		printf("get TriggerSoftware node fail.\n");
		return false;
	}
	if (false == nodeTriggerSoftware.execute())
	{
		printf("set TriggerSoftware fail.\n");
		return false;
	}

	printf("ExecuteSoftTrig success.\n");
	return true;
}

// 设置当前相机  
// set current camera
void CammerWidget::SetCamera(const QString& strKey)
{
	CSystem& systemObj = CSystem::getInstance();
	pCamera = systemObj.getCameraPtr(strKey.toStdString().c_str());
	sptrFormatControl = systemObj.createImageFormatControl(pCamera);
	acquisitionControl = systemObj.createAcquisitionControl(pCamera);
}

// 显示
// diaplay
bool CammerWidget::ShowImage(uint8_t* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat)
{
	// 默认显示30帧
	// default display 30 frames 
	if (NULL == pRgbFrameBuf || nWidth == 0 || nHeight == 0)
	{
		printf("%s image is invalid.\n", __FUNCTION__);
		return false;
	}

	// 创建图像并赋值给成员变量
	if (Dahua::GenICam::gvspPixelMono8 == nPixelFormat)
	{
		image = QImage(pRgbFrameBuf, nWidth, nHeight, QImage::Format_Grayscale8);
	}
	else
	{
		image = QImage(pRgbFrameBuf, nWidth, nHeight, QImage::Format_RGB888);
	}

	//targetRect = QRectF(0, 0, nWidth * scaleFactor, nHeight * scaleFactor);
	//sourceRect = QRectF(0, 0, nWidth, nHeight); // 确保源矩形与图像匹配
	if (!imageSet) {
		setImage(image);
		imageSet = true; // 更新标志，表示图像已经设置过
	}
	return true;
}

// 显示线程
// display thread
void CammerWidget::DisplayThreadProc(Dahua::Infra::CThreadLite& lite)
{
	while (lite.looping())
	{
		CFrameInfo frameInfo;

		if (false == qDisplayFrameQueue.get(frameInfo, 500))
		{
			continue;
		}

		// 判断是否要显示。超过显示上限（30帧），就不做转码、显示处理
		// Judge whether to display. If the upper display limit (30 frames) is exceeded, transcoding and display processing will not be performed
		if (!isTimeToDisplay())
		{
			// 释放内存
			// release memory
			free(frameInfo.pImageBuf);
			continue;
		}

		// mono8格式可不做转码，直接显示，其他格式需要经过转码才能显示 
		// mono8 format can be displayed directly without transcoding. Other formats can be displayed only after transcoding
		if (Dahua::GenICam::gvspPixelMono8 == frameInfo.ePixelType)
		{
			// 显示线程中发送显示信号，在主线程中显示图像
			// Send display signal in display thread and display image in main thread
			emit signalShowImage(frameInfo.pImageBuf, frameInfo.nWidth, frameInfo.nHeight, frameInfo.ePixelType);

		}
		else
		{
			// 转码
			// transcoding
			uint8_t* pRGBbuffer = NULL;
			int nRgbBufferSize = 0;
			nRgbBufferSize = frameInfo.nWidth * frameInfo.nHeight * 3;
			pRGBbuffer = (uint8_t*)malloc(nRgbBufferSize);
			if (pRGBbuffer == NULL)
			{
				// 释放内存
				// release memory
				free(frameInfo.pImageBuf);
				printf("RGBbuffer malloc failed.\n");
				continue;
			}

			IMGCNV_SOpenParam openParam;
			openParam.width = frameInfo.nWidth;
			openParam.height = frameInfo.nHeight;
			openParam.paddingX = frameInfo.nPaddingX;
			openParam.paddingY = frameInfo.nPaddingY;
			openParam.dataSize = frameInfo.nBufferSize;
			openParam.pixelForamt = frameInfo.ePixelType;

			IMGCNV_EErr status = IMGCNV_ConvertToRGB24(frameInfo.pImageBuf, &openParam, pRGBbuffer, &nRgbBufferSize);
			if (IMGCNV_SUCCESS != status)
			{
				// 释放内存 
				// release memory
				printf("IMGCNV_ConvertToRGB24 failed.\n");
				free(frameInfo.pImageBuf);
				free(pRGBbuffer);
				return;
			}

			// 释放内存
			// release memory
			free(frameInfo.pImageBuf);

			// 显示线程中发送显示信号，在主线程中显示图像
			// Send display signal in display thread and display image in main thread
			emit signalShowImage(pRGBbuffer, openParam.width, openParam.height, openParam.pixelForamt);

		}
	}
}

bool CammerWidget::isTimeToDisplay()
{
	CGuard guard(mxTime);

	// 不显示
	// don't display
	if (nDisplayInterval <= 0)
	{
		return false;
	}

	// 第一帧必须显示
	// the frist frame must be displayed
	if (nFirstFrameTime == 0 || nLastFrameTime == 0)
	{
		nFirstFrameTime = elapsedTimer.nsecsElapsed();
		nLastFrameTime = nFirstFrameTime;

		return true;
	}

	// 当前帧和上一帧的间隔如果大于显示间隔就显示
	// display if the interval between the current frame and the previous frame is greater than the display interval
	uint64_t nCurTimeTmp = elapsedTimer.nsecsElapsed();
	uint64_t nAcquisitionInterval = nCurTimeTmp - nLastFrameTime;
	if (nAcquisitionInterval > nDisplayInterval)
	{
		nLastFrameTime = nCurTimeTmp;
		return true;
	}

	// 当前帧相对于第一帧的时间间隔
	// Time interval between the current frame and the first frame
	uint64_t nPre = (nLastFrameTime - nFirstFrameTime) % nDisplayInterval;
	if (nPre + nAcquisitionInterval > nDisplayInterval)
	{
		nLastFrameTime = nCurTimeTmp;
		return true;
	}

	return false;
}

// 设置显示频率
// set display frequency
void CammerWidget::setDisplayFPS(int nFPS)
{
	showRate = nFPS;
	if (showRate > 0)
	{
		CGuard guard(mxTime);

		nDisplayInterval = 1000 * 1000 * 1000.0 / showRate;
	}
	else
	{
		CGuard guard(mxTime);
		nDisplayInterval = 0;
	}
}

// 窗口关闭响应函数
// window close response function
void CammerWidget::closeEvent(QCloseEvent* event)
{
	if (NULL != pStreamSource && pStreamSource->isGrabbing())
		pStreamSource->stopGrabbing();
	if (NULL != pCamera && pCamera->isConnected())
		pCamera->disConnect();
}

// 状态栏统计信息 开始
// Status bar statistics begin
void CammerWidget::resetStatistic()
{
	QMutexLocker locker(&mxStatistic);
	nTotalFrameCount = 0;
	listFrameStatInfo.clear();
	bNeedUpdate = true;
}
QString CammerWidget::getStatistic()
{
	if (mxStatistic.tryLock(30))
	{
		if (bNeedUpdate)
		{
			updateStatistic();
		}

		mxStatistic.unlock();
		return strStatistic;
	}
	return "";
}
void CammerWidget::updateStatistic()
{
	size_t nFrameCount = listFrameStatInfo.size();
	QString strFPS = DEFAULT_ERROR_STRING;
	QString strSpeed = DEFAULT_ERROR_STRING;

	if (nFrameCount > 1)
	{
		quint64 nTotalSize = 0;
		FrameList::const_iterator it = listFrameStatInfo.begin();

		if (listFrameStatInfo.size() == 2)
		{
			nTotalSize = listFrameStatInfo.back().m_nFrameSize;
		}
		else
		{
			for (++it; it != listFrameStatInfo.end(); ++it)
			{
				nTotalSize += it->m_nFrameSize;
			}
		}

		const FrameStatInfo& first = listFrameStatInfo.front();
		const FrameStatInfo& last = listFrameStatInfo.back();

		qint64 nsecs = last.m_nPassTime - first.m_nPassTime;

		if (nsecs > 0)
		{
			double dFPS = (nFrameCount - 1) * ((double)1000000000.0 / nsecs);
			double dSpeed = nTotalSize * ((double)1000000000.0 / nsecs) / (1000.0) / (1000.0) * (8.0);
			strFPS = QString::number(dFPS, 'f', 2);
			strSpeed = QString::number(dSpeed, 'f', 2);
		}
	}

	strStatistic = QString("Stream: %1 images   %2 FPS   %3 Mbps")
		.arg(nTotalFrameCount)
		.arg(strFPS)
		.arg(strSpeed);
	bNeedUpdate = false;
}
void CammerWidget::recvNewFrame(const CFrame& pBuf)
{
	QMutexLocker locker(&mxStatistic);
	if (listFrameStatInfo.size() >= MAX_FRAME_STAT_NUM)
	{
		listFrameStatInfo.pop_front();
	}
	listFrameStatInfo.push_back(FrameStatInfo(pBuf.getImageSize(), elapsedTimer.nsecsElapsed()));
	++nTotalFrameCount;

	if (listFrameStatInfo.size() > MIN_LEFT_LIST_NUM)
	{
		FrameStatInfo infoFirst = listFrameStatInfo.front();
		FrameStatInfo infoLast = listFrameStatInfo.back();
		while (listFrameStatInfo.size() > MIN_LEFT_LIST_NUM && infoLast.m_nPassTime - infoFirst.m_nPassTime > MAX_STATISTIC_INTERVAL)
		{
			listFrameStatInfo.pop_front();
			infoFirst = listFrameStatInfo.front();
		}
	}

	bNeedUpdate = true;
}

// 更新帧率
void CammerWidget::updateShowRate(double value)
{
	double maxFrameRate = 2000.0;
	acquisition = value;
	CDoubleNode frameRate = acquisitionControl->acquisitionFrameRate();
	CBoolNode frameRateEnableNode = acquisitionControl->acquisitionFrameRateEnable();
	if (acquisition < maxFrameRate) {
		frameRate.setValue(acquisition);
	}
	else {
		frameRate.setValue(maxFrameRate);
	}
	frameRateEnableNode.setValue(true);
}

//缩放
void CammerWidget::zoomIn() {
	// 增加缩放因子
	zoomFactor *= 1.1;
	update();
}

void CammerWidget::zoomOut() {
	// 减少缩放因子
	zoomFactor /= 1.1;
	update();
}

void CammerWidget::resolution(int width, int height)
{
	qwidth = width;
	qheight = height;
	// 获取宽度和高度节点
	CIntNode nodeWidth = sptrFormatControl->width();
	CIntNode nodeHeight = sptrFormatControl->height();
	nodeWidth.setValue(qwidth);
	nodeHeight.setValue(qheight);
}

//重绘
void CammerWidget::setImage(const QImage& newImage)
{
	image = newImage;

	if (!image.isNull()) {
		// 计算适合小部件的缩放因子
		float widthScale = width() / static_cast<float>(image.width());
		float heightScale = height() / static_cast<float>(image.height());
		scaleFactor = qMin(widthScale, heightScale); // 选择较小的缩放因子，保持图像比例

		// 设置偏移为0，以确保图像在小部件中居中
		imageOffset = QPointF((width() - image.width() * scaleFactor) / 2,
			(height() - image.height() * scaleFactor) / 2);
	}
	update(); // 更新小部件以触发重绘
}

void CammerWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	if (!image.isNull()) {
		// 计算缩放后的图像大小
		QSizeF scaledSize = image.size() * scaleFactor;
		QPointF topLeft = -imageOffset; // 使用 (0, 0) 作为起点
		QRectF drawRect(topLeft, scaledSize);
		painter.drawImage(drawRect, image);
	}
}

//捕获鼠标点击位置
void CammerWidget::mousePressEvent(QMouseEvent* event)
{
	QPointF clickPos = event->pos();

	// 计算鼠标点击位置相对于图像中心的偏移
	QPointF relativePos = clickPos - (-imageOffset); // 以 (0, 0) 作为中心

	// 更新缩放因子
	float oldScaleFactor = scaleFactor;
	scaleFactor *= 1.1f; // 放大1.1倍

	// 更新偏移，保持鼠标位置不变
	imageOffset = (relativePos * (scaleFactor / oldScaleFactor)) - relativePos + imageOffset;

	update(); // 更新小部件以触发重绘
}
void CammerWidget::wheelEvent(QWheelEvent* event)
{
	QPointF mousePos = event->position();

	// 计算鼠标位置相对于图像中心的偏移
	QPointF relativePos = mousePos - (-imageOffset); // 以 (0, 0) 作为中心

	// 更新缩放因子
	float oldScaleFactor = scaleFactor;
	if (event->angleDelta().y() > 0) {
		scaleFactor *= 1.1f; // 放大
	}
	else {
		scaleFactor /= 1.1f; // 缩小
	}

	// 更新偏移，保持鼠标位置不变
	imageOffset = (relativePos * (scaleFactor / oldScaleFactor)) - relativePos + imageOffset;

	update(); // 更新小部件以触发重绘
}

// 状态栏统计信息 end 
// Status bar statistics ending