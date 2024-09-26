#include "CammerWidget.h"
#include "ui_cammerwidget.h"
#include "GenICam/System.h"
#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"
#include <qpainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer.h>

/*#define DEFAULT_SHOW_RATE (60)*/ // 默认显示帧率 | defult display frequency
#define DEFAULT_ERROR_STRING ("N/A") 
#define MAX_FRAME_STAT_NUM (50) 
#define MIN_LEFT_LIST_NUM (2) 
#define MAX_STATISTIC_INTERVAL (5000000000) // List的更新时与最新一帧的时间最大间隔 |  The maximum time interval between the update of list and the latest frame

int g_lShowRate = 30;
int g_lWidth = 1920, g_lHeight = 1024, g_lOffsetX = 0, g_lOffsetY = 0;
int g_lAcquisition = 30;
const float m_fMinScaleFactor = 0.2f;
QPointF m_startPoint;
QPointF m_endPoint;
bool m_isCropping = false;

using namespace Dahua::GenICam;
using namespace Dahua::Infra;

CammerWidget::CammerWidget(QWidget* parent) :
	QWidget(parent)
	, ui(new Ui::CammerWidget)
	, m_thdDisplayThread(CThreadLite::ThreadProc(&CammerWidget::DisplayThreadProc, this), "Display")
	, m_nDisplayInterval(0)
	, m_nFirstFrameTime(0)
	, m_nLastFrameTime(0)
	, m_bNeedUpdate(true)
	, m_nTotalFrameCount(0),
	m_bImageSet(false)
{
	ui->setupUi(this);

	qRegisterMetaType<uint64_t>("uint64_t");
	connect(this, SIGNAL(signalShowImage(uint8_t*, int, int, uint64_t)), this, SLOT(ShowImage(uint8_t*, int, int, uint64_t)));

	setDisplayFPS(g_lShowRate);
	m_elapsedTimer.start();

	// 启动显示线程
	// start display thread
	if (!m_thdDisplayThread.isThreadOver())
	{
		m_thdDisplayThread.destroyThread();
	}
	m_thdDisplayThread.createThread();
}

CammerWidget::~CammerWidget()
{
	// 关闭显示线程
	// close display thread
	if (!m_thdDisplayThread.isThreadOver())
	{
		m_thdDisplayThread.destroyThread();
	}

	delete ui;
}

// 取流回调函数
// get frame callback function 
void CammerWidget::FrameCallback(const CFrame& frame)
{
	CFrameInfo frameInfo;
	frameInfo.m_nWidth = frame.getImageWidth();
	frameInfo.m_nHeight = frame.getImageHeight();
	frameInfo.m_nBufferSize = frame.getImageSize();
	frameInfo.m_nPaddingX = frame.getImagePadddingX();
	frameInfo.m_nPaddingY = frame.getImagePadddingY();
	frameInfo.m_ePixelType = frame.getImagePixelFormat();
	frameInfo.m_pImageBuf = (BYTE*)malloc(sizeof(BYTE) * frameInfo.m_nBufferSize);
	frameInfo.m_nTimeStamp = frame.getImageTimeStamp();

	// 内存申请失败，直接返回
	// memory application failed, return directly
	if (frameInfo.m_pImageBuf != NULL)
	{
		memcpy(frameInfo.m_pImageBuf, frame.getImage(), frame.getImageSize());

		if (m_qDisplayFrameQueue.size() > 16)
		{
			CFrameInfo frameOld;
			m_qDisplayFrameQueue.get(frameOld);
			free(frameOld.m_pImageBuf);
		}

		m_qDisplayFrameQueue.push_back(frameInfo);
	}

	recvNewFrame(frame);
}

//单帧处理
void CammerWidget::FrameSingle(const CFrame& frame)
{
	CFrameInfo frameInfo;
	frameInfo.m_nWidth = frame.getImageWidth();
	frameInfo.m_nHeight = frame.getImageHeight();
	frameInfo.m_nBufferSize = frame.getImageSize();
	frameInfo.m_nPaddingX = frame.getImagePadddingX();
	frameInfo.m_nPaddingY = frame.getImagePadddingY();
	frameInfo.m_ePixelType = frame.getImagePixelFormat();
	frameInfo.m_pImageBuf = (BYTE*)malloc(sizeof(BYTE) * frameInfo.m_nBufferSize);
	frameInfo.m_nTimeStamp = frame.getImageTimeStamp();

	// 内存申请失败，直接返回
	// memory application failed, return directly
	if (frameInfo.m_pImageBuf != NULL)
	{
		memcpy(frameInfo.m_pImageBuf, frame.getImage(), frame.getImageSize());

		// 只处理第一帧
		if (m_qDisplayFrameQueue.size() == 0)  // 使用 size() 检查是否为空
		{
			m_qDisplayFrameQueue.push_back(frameInfo);
			// 停止抓取
			if (m_pStreamSource)
			{
				m_pStreamSource->stopGrabbing();
			}
		}
		else
		{
			free(frameInfo.m_pImageBuf); // 如果有多余的帧数据，释放内存
		}
	}

	recvNewFrame(frame);
}

// 设置曝光
// set exposeTime
bool CammerWidget::SetExposeTime(double dExposureTime)
{
	if (NULL == m_pCamera)
	{
		printf("Set ExposureTime fail. No camera or camera is not connected.\n");
		return false;
	}

	CDoubleNode nodeExposureTime(m_pCamera, "ExposureTime");

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
	if (NULL == m_pCamera)
	{
		printf("Set GainRaw fail. No camera or camera is not connected.\n");
		return false;
	}

	CDoubleNode nodeGainRaw(m_pCamera, "GainRaw");

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
	if (NULL == m_pCamera)
	{
		printf("connect camera fail. No camera.\n");
		return false;
	}

	if (true == m_pCamera->isConnected())
	{
		printf("camera is already connected.\n");
		return true;
	}

	if (false == m_pCamera->connect())
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
	if (NULL == m_pCamera)
	{
		printf("disconnect camera fail. No camera.\n");
		return false;
	}

	if (false == m_pCamera->isConnected())
	{
		printf("camera is already disconnected.\n");
		return true;
	}

	if (false == m_pCamera->disConnect())
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
	if (NULL == m_pStreamSource)
	{
		m_pStreamSource = CSystem::getInstance().createStreamSource(m_pCamera);
	}

	if (NULL == m_pStreamSource)
	{
		return false;
	}

	if (m_pStreamSource->isGrabbing())
	{
		return true;
	}

	bool bRet = m_pStreamSource->attachGrabbing(IStreamSource::Proc(&CammerWidget::FrameCallback, this));
	if (!bRet)
	{
		return false;
	}

	if (!m_pStreamSource->startGrabbing())
	{
		return false;
	}

	return true;
}

//单张采集
bool CammerWidget::CaptureSingleImage()
{
	if (NULL == m_pStreamSource)
	{
		m_pStreamSource = CSystem::getInstance().createStreamSource(m_pCamera);
	}

	if (NULL == m_pStreamSource)
	{
		return false;
	}

	if (m_pStreamSource->isGrabbing())
	{
		return true;
	}

	bool bRet = m_pStreamSource->attachGrabbing(IStreamSource::Proc(&CammerWidget::FrameSingle, this));
	if (!bRet)
	{
		return false;
	}

	if (!m_pStreamSource->startGrabbing())
	{
		return false;
	}

	return true;
}


// 停止采集
// stop grabbing
bool CammerWidget::CameraStop()
{
	if (m_pStreamSource != NULL)
	{
		m_pStreamSource->detachGrabbing(IStreamSource::Proc(&CammerWidget::FrameCallback, this));

		m_pStreamSource->stopGrabbing();
		m_pStreamSource.reset();
	}

	// 清空显示队列 
	// clear display queue
	m_qDisplayFrameQueue.clear();

	return true;
}

// 切换采集方式、触发方式 （连续采集、外部触发、软件触发）
// Switch acquisition mode and triggering mode (continuous acquisition, external triggering and software triggering)
bool CammerWidget::CameraChangeTrig(ETrigType trigType)
{
	if (NULL == m_pCamera)
	{
		printf("Change Trig fail. No camera or camera is not connected.\n");
		return false;
	}

	if (trigContinous == trigType)
	{
		// 设置触发模式
		// set trigger mode
		CEnumNode nodeTriggerMode(m_pCamera, "TriggerMode");
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
		CEnumNode nodeTriggerSelector(m_pCamera, "TriggerSelector");
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
		CEnumNode nodeTriggerMode(m_pCamera, "TriggerMode");
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
		CEnumNode nodeTriggerSource(m_pCamera, "TriggerSource");
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
		CEnumNode nodeTriggerSelector(m_pCamera, "TriggerSelector");
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
		CEnumNode nodeTriggerMode(m_pCamera, "TriggerMode");
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
		CEnumNode nodeTriggerSource(m_pCamera, "TriggerSource");
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
	if (NULL == m_pCamera)
	{
		printf("Set GainRaw fail. No camera or camera is not connected.\n");
		return false;
	}

	CCmdNode nodeTriggerSoftware(m_pCamera, "TriggerSoftware");
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
	m_pCamera = systemObj.getCameraPtr(strKey.toStdString().c_str());
	m_pSptrFormatControl = systemObj.createImageFormatControl(m_pCamera);
	m_pAcquisitionControl = systemObj.createAcquisitionControl(m_pCamera);

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
		m_aImage = QImage(pRgbFrameBuf, nWidth, nHeight, QImage::Format_Grayscale8);
	}
	else
	{
		m_aImage = QImage(pRgbFrameBuf, nWidth, nHeight, QImage::Format_RGB888);
	}

	if (!m_bImageSet) {
		setImage(m_aImage);
		m_bImageSet = true; // 更新标志，表示图像已经设置过
	}
	update();
	return true;
}

// 显示线程
// display thread
void CammerWidget::DisplayThreadProc(Dahua::Infra::CThreadLite& lite)
{
	while (lite.looping())
	{
		CFrameInfo frameInfo;

		if (false == m_qDisplayFrameQueue.get(frameInfo, 500))
		{
			continue;
		}

		// 判断是否要显示。超过显示上限（30帧），就不做转码、显示处理
		// Judge whether to display. If the upper display limit (30 frames) is exceeded, transcoding and display processing will not be performed
		if (!isTimeToDisplay())
		{
			// 释放内存
			// release memory
			free(frameInfo.m_pImageBuf);
			continue;
		}

		// mono8格式可不做转码，直接显示，其他格式需要经过转码才能显示 
		// mono8 format can be displayed directly without transcoding. Other formats can be displayed only after transcoding
		if (Dahua::GenICam::gvspPixelMono8 == frameInfo.m_ePixelType)
		{
			// 显示线程中发送显示信号，在主线程中显示图像
			// Send display signal in display thread and display image in main thread
			emit signalShowImage(frameInfo.m_pImageBuf, frameInfo.m_nWidth, frameInfo.m_nHeight, frameInfo.m_ePixelType);

		}
		else
		{
			// 转码
			// transcoding
			uint8_t* pRGBbuffer = NULL;
			int nRgbBufferSize = 0;
			nRgbBufferSize = frameInfo.m_nWidth * frameInfo.m_nHeight * 3;
			pRGBbuffer = (uint8_t*)malloc(nRgbBufferSize);
			if (pRGBbuffer == NULL)
			{
				// 释放内存
				// release memory
				free(frameInfo.m_pImageBuf);
				printf("RGBbuffer malloc failed.\n");
				continue;
			}

			IMGCNV_SOpenParam openParam;
			openParam.width = frameInfo.m_nWidth;
			openParam.height = frameInfo.m_nHeight;
			openParam.paddingX = frameInfo.m_nPaddingX;
			openParam.paddingY = frameInfo.m_nPaddingY;
			openParam.dataSize = frameInfo.m_nBufferSize;
			openParam.pixelForamt = frameInfo.m_ePixelType;

			IMGCNV_EErr status = IMGCNV_ConvertToRGB24(frameInfo.m_pImageBuf, &openParam, pRGBbuffer, &nRgbBufferSize);
			if (IMGCNV_SUCCESS != status)
			{
				// 释放内存 
				// release memory
				printf("IMGCNV_ConvertToRGB24 failed.\n");
				free(frameInfo.m_pImageBuf);
				free(pRGBbuffer);
				return;
			}

			// 释放内存
			// release memory
			free(frameInfo.m_pImageBuf);

			// 显示线程中发送显示信号，在主线程中显示图像
			// Send display signal in display thread and display image in main thread
			emit signalShowImage(pRGBbuffer, openParam.width, openParam.height, openParam.pixelForamt);

		}
	}
}

bool CammerWidget::isTimeToDisplay()
{
	CGuard guard(m_mxTime);

	// 不显示
	// don't display
	if (m_nDisplayInterval <= 0)
	{
		return false;
	}

	// 第一帧必须显示
	// the frist frame must be displayed
	if (m_nFirstFrameTime == 0 || m_nLastFrameTime == 0)
	{
		m_nFirstFrameTime = m_elapsedTimer.nsecsElapsed();
		m_nLastFrameTime = m_nFirstFrameTime;

		return true;
	}

	// 当前帧和上一帧的间隔如果大于显示间隔就显示
	// display if the interval between the current frame and the previous frame is greater than the display interval
	uint64_t nCurTimeTmp = m_elapsedTimer.nsecsElapsed();
	uint64_t nAcquisitionInterval = nCurTimeTmp - m_nLastFrameTime;
	if (nAcquisitionInterval > m_nDisplayInterval)
	{
		m_nLastFrameTime = nCurTimeTmp;
		return true;
	}

	// 当前帧相对于第一帧的时间间隔
	// Time interval between the current frame and the first frame
	uint64_t nPre = (m_nLastFrameTime - m_nFirstFrameTime) % m_nDisplayInterval;
	if (nPre + nAcquisitionInterval > m_nDisplayInterval)
	{
		m_nLastFrameTime = nCurTimeTmp;
		return true;
	}

	return false;
}

// 设置显示频率
// set display frequency
void CammerWidget::setDisplayFPS(int nFPS)
{
	g_lShowRate = nFPS;
	if (g_lShowRate > 0)
	{
		CGuard guard(m_mxTime);

		m_nDisplayInterval = 1000 * 1000 * 1000.0 / g_lShowRate;
	}
	else
	{
		CGuard guard(m_mxTime);
		m_nDisplayInterval = 0;
	}
}

// 窗口关闭响应函数
// window close response function
void CammerWidget::closeEvent(QCloseEvent* event)
{
	if (NULL != m_pStreamSource && m_pStreamSource->isGrabbing())
		m_pStreamSource->stopGrabbing();
	if (NULL != m_pCamera && m_pCamera->isConnected())
		m_pCamera->disConnect();
}

// 状态栏统计信息 开始
// Status bar statistics begin
void CammerWidget::resetStatistic()
{
	QMutexLocker locker(&m_mxStatistic);
	m_nTotalFrameCount = 0;
	m_listFrameStatInfo.clear();
	m_bNeedUpdate = true;
}
QString CammerWidget::getStatistic()
{
	if (m_mxStatistic.tryLock(30))
	{
		if (m_bNeedUpdate)
		{
			updateStatistic();
		}

		m_mxStatistic.unlock();
		return m_szStatistic;
	}
	return "";
}
void CammerWidget::updateStatistic()
{
	size_t nFrameCount = m_listFrameStatInfo.size();
	QString strFPS = DEFAULT_ERROR_STRING;
	QString strSpeed = DEFAULT_ERROR_STRING;

	if (nFrameCount > 1)
	{
		quint64 nTotalSize = 0;
		FrameList::const_iterator it = m_listFrameStatInfo.begin();

		if (m_listFrameStatInfo.size() == 2)
		{
			nTotalSize = m_listFrameStatInfo.back().m_nFrameSize;
		}
		else
		{
			for (++it; it != m_listFrameStatInfo.end(); ++it)
			{
				nTotalSize += it->m_nFrameSize;
			}
		}

		const FrameStatInfo& first = m_listFrameStatInfo.front();
		const FrameStatInfo& last = m_listFrameStatInfo.back();

		qint64 nsecs = last.m_nPassTime - first.m_nPassTime;

		if (nsecs > 0)
		{
			double dFPS = (nFrameCount - 1) * ((double)1000000000.0 / nsecs);
			double dSpeed = nTotalSize * ((double)1000000000.0 / nsecs) / (1000.0) / (1000.0) * (8.0);
			strFPS = QString::number(dFPS, 'f', 2);
			strSpeed = QString::number(dSpeed, 'f', 2);
		}
	}

	m_szStatistic = QString("Stream: %1 images   %2 FPS   %3 Mbps")
		.arg(m_nTotalFrameCount)
		.arg(strFPS)
		.arg(strSpeed);
	m_bNeedUpdate = false;
}
void CammerWidget::recvNewFrame(const CFrame& pBuf)
{
	QMutexLocker locker(&m_mxStatistic);
	if (m_listFrameStatInfo.size() >= MAX_FRAME_STAT_NUM)
	{
		m_listFrameStatInfo.pop_front();
	}
	m_listFrameStatInfo.push_back(FrameStatInfo(pBuf.getImageSize(), m_elapsedTimer.nsecsElapsed()));
	++m_nTotalFrameCount;

	if (m_listFrameStatInfo.size() > MIN_LEFT_LIST_NUM)
	{
		FrameStatInfo infoFirst = m_listFrameStatInfo.front();
		FrameStatInfo infoLast = m_listFrameStatInfo.back();
		while (m_listFrameStatInfo.size() > MIN_LEFT_LIST_NUM && infoLast.m_nPassTime - infoFirst.m_nPassTime > MAX_STATISTIC_INTERVAL)
		{
			m_listFrameStatInfo.pop_front();
			infoFirst = m_listFrameStatInfo.front();
		}
	}

	m_bNeedUpdate = true;
}

// 更新帧率
void CammerWidget::updateShowRate(double value)
{
	double l_iMaxFrameRate = 2000.0;
	g_lAcquisition = value;
	CDoubleNode frameRate = m_pAcquisitionControl->acquisitionFrameRate();
	CBoolNode frameRateEnableNode = m_pAcquisitionControl->acquisitionFrameRateEnable();
	if (g_lAcquisition < l_iMaxFrameRate) {
		frameRate.setValue(g_lAcquisition);
	}
	else {
		frameRate.setValue(l_iMaxFrameRate);
	}
	frameRateEnableNode.setValue(true);
}

//设置宽度
void CammerWidget::setWidth(int width)
{
	CameraStop();
	g_lWidth = width;
	CIntNode nodeWidth = m_pSptrFormatControl->width();
	int adjustedX = (g_lWidth + 15) / 16 * 16;
	nodeWidth.setValue(adjustedX);
	CameraStart();
}

//设置高度
void CammerWidget::setHeight(int height)
{
	CameraStop();
	g_lHeight = height;
	CIntNode nodeWidth = m_pSptrFormatControl->height();
	nodeWidth.setValue(g_lHeight);
	CameraStart();
}

//设置X偏移量
void CammerWidget::setOffsetX(int offsetX) {
	CameraStop();
	g_lOffsetX = offsetX;
	CIntNode nodeOffsetX = m_pSptrFormatControl->offsetX();
	int adjustedWidth = (g_lOffsetX + 15) / 16 * 16;
	nodeOffsetX.setValue(adjustedWidth);
	CameraStart();
}

//设置Y偏移量
void CammerWidget::setOffsetY(int offsetY) {
	CameraStop();
	g_lOffsetY = offsetY;
	CIntNode nodeOffsetY = m_pSptrFormatControl->offsetY();
	nodeOffsetY.setValue(g_lOffsetY);
	CameraStart();
}

//设置最大分辨率
void CammerWidget::setMaxResolution() {
	CameraStop();
	CIntNode offsetX = m_pSptrFormatControl->offsetX();
	CIntNode offsetY = m_pSptrFormatControl->offsetY();
	CIntNode nodeWidth = m_pSptrFormatControl->width();
	CIntNode nodeHeight = m_pSptrFormatControl->height();
	offsetX.setValue(0);
	offsetY.setValue(0);
	nodeWidth.setValue(2592);
	nodeHeight.setValue(2048);

	// 重置缩放因子和偏移量
	m_fScaleFactor = 1.0f;
	m_pImageOffset = QPointF(0, 0);
	g_lOffsetX = 0;
	g_lOffsetY = 0();

	// 设置一个标志，表示我们刚刚重置了分辨率
	m_bJustResetResolution = true;

	CameraStart();
	QTimer::singleShot(50, this, [this]() {
		setImage(m_aImage);
		});
}

// 修改 setImage 函数以处理分辨率重置
void CammerWidget::setImage(const QImage& newImage)
{
	m_aImage = newImage;

	if (!m_aImage.isNull()) {
		if (m_bJustResetResolution) {
			// 使用适合窗口的缩放因子
			float widthScale = width() / static_cast<float>(m_aImage.width());
			float heightScale = height() / static_cast<float>(m_aImage.height());
			m_fScaleFactor = qMin(widthScale, heightScale);
			m_pImageOffset = QPointF(0, 0);
			m_bJustResetResolution = false;  // 重置标志
		}
		// 否则，保持当前的缩放因子和偏移量
	}
	update(); // 更新小部件以触发重绘
}

//捕获鼠标点击位置
void CammerWidget::mousePressEvent(QMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		m_startPoint = event->pos();
		m_endPoint = m_startPoint;
		m_isCropping = true;
		update();
	}
}

// 捕获鼠标移动位置
void CammerWidget::mouseMoveEvent(QMouseEvent* event) {
	if (m_isCropping) {
		m_endPoint = event->pos();
		update();
	}
}

// 捕获鼠标释放位置
void CammerWidget::mouseReleaseEvent(QMouseEvent* event) {
	CameraStop();
	m_endPoint = event->pos();
	m_isCropping = false;

	QRect cropRect = calculateCropRect();
	applyCrop(cropRect);

	CameraStart();
	QTimer::singleShot(50, this, [this]() {
		setImage(m_aImage);
		});
}

void CammerWidget::alignTopLeft() {
	m_pImageOffset = QPointF(0, 0);
}

// 修改 wheelEvent 以根据鼠标位置缩放，同时保持左上角对齐
void CammerWidget::wheelEvent(QWheelEvent* event) {
	QPointF mousePos = event->position();
	float oldScaleFactor = m_fScaleFactor;

	if (event->angleDelta().y() > 0) {
		m_fScaleFactor *= 1.1f;
	}
	else {
		m_fScaleFactor /= 1.1f;
	}

	m_fScaleFactor = qMax(m_fScaleFactor, m_fMinScaleFactor);

	// 计算鼠标位置相对于图像的比例
	QPointF relativePos = (mousePos - m_pImageOffset) / oldScaleFactor;

	// 计算新的偏移，以保持鼠标指向的图像点不变
	QPointF newOffset = mousePos - relativePos * m_fScaleFactor;

	// 调整偏移以保持左上角对齐，同时不超出图像边界
	m_pImageOffset.setX(qMin(static_cast<double>(qMax(static_cast<double>(newOffset.x()),
		static_cast<double>(width() - m_aImage.width() * m_fScaleFactor))), 0.0));

	m_pImageOffset.setY(qMin(static_cast<double>(qMax(static_cast<double>(newOffset.y()),
		static_cast<double>(height() - m_aImage.height() * m_fScaleFactor))), 0.0));

	update();
	event->accept();
}

// 更新 paintEvent 函数以确保正确绘制
void CammerWidget::paintEvent(QPaintEvent* event) {
	QPainter painter(this);

	if (!m_aImage.isNull()) {
		QSizeF scaledSize = m_aImage.size() * m_fScaleFactor;
		QRectF drawRect(m_pImageOffset, scaledSize);

		// 绘制图像
		painter.drawImage(drawRect, m_aImage);

		// 绘制裁剪选框
		if (m_isCropping) {
			QRectF cropRect = QRectF(m_startPoint, m_endPoint).normalized();
			painter.setPen(Qt::red);
			painter.drawRect(cropRect);
		}
	}
}

// 更新 calculateCropRect 函数以考虑新的缩放和偏移
QRect CammerWidget::calculateCropRect() {
	QPointF topLeft = m_pImageOffset;
	QPointF bottomRight = topLeft + QPointF(m_aImage.width() * m_fScaleFactor, m_aImage.height() * m_fScaleFactor);

	int l_iLeft = qMax(qMin(m_startPoint.x(), m_endPoint.x()), static_cast<int>(topLeft.x()));
	int l_iTop = qMax(qMin(m_startPoint.y(), m_endPoint.y()), static_cast<int>(topLeft.y()));
	int l_iRight = qMin(qMax(m_startPoint.x(), m_endPoint.x()), static_cast<int>(bottomRight.x()));
	int l_iBottom = qMin(qMax(m_startPoint.y(), m_endPoint.y()), static_cast<int>(bottomRight.y()));

	int l_iScaledLeft = (l_iLeft - topLeft.x()) / m_fScaleFactor;
	int l_iScaledTop = (l_iTop - topLeft.y()) / m_fScaleFactor;
	int l_iScaledRight = (l_iRight - topLeft.x()) / m_fScaleFactor;
	int l_iScaledBottom = (l_iBottom - topLeft.y()) / m_fScaleFactor;

	QRect cropRect(l_iScaledLeft, l_iScaledTop,
		l_iScaledRight - l_iScaledLeft,
		l_iScaledBottom - l_iScaledTop);

	return cropRect.intersected(QRect(0, 0, m_aImage.width(), m_aImage.height()));
}

QRect CammerWidget::applyCrop(const QRect& cropRect) {
	g_lOffsetX += cropRect.x();
	g_lOffsetY += cropRect.y();
	g_lWidth = cropRect.width();
	g_lHeight = cropRect.height();

	int l_iAdjustedX = (g_lOffsetX + 15) / 16 * 16;
	int l_iAdjustedWidth = (g_lWidth + 15) / 16 * 16;

	m_pSptrFormatControl->width().setValue(l_iAdjustedWidth);
	m_pSptrFormatControl->height().setValue(g_lHeight);
	m_pSptrFormatControl->offsetX().setValue(l_iAdjustedX);
	m_pSptrFormatControl->offsetY().setValue(g_lOffsetY);

	// 更新图像
	m_aImage = m_aImage.copy(cropRect);

	// 重置缩放和偏移
	m_fScaleFactor = 1.0f;
	m_pImageOffset = QPointF(0, 0);

	update();
	return cropRect;
}

void CammerWidget::resetImage()
{
	// 重置图像
	setImage(m_aImage);
}

//属性显示
int CammerWidget::getHeight()
{
	CIntNode nodeHeight = m_pSptrFormatControl->height();
	int64_t nCurVal = 0;
	nodeHeight.getValue(nCurVal);
	return nCurVal;
}

int CammerWidget::getWidth()
{
	CIntNode nodeWidth = m_pSptrFormatControl->width();
	int64_t nCurVal = 0;
	nodeWidth.getValue(nCurVal);
	return nCurVal;
}

int CammerWidget::getOffsetX()
{
	CIntNode nodeOffsetX = m_pSptrFormatControl->offsetX();
	int64_t nCurVal = 0;
	nodeOffsetX.getValue(nCurVal);
	return nCurVal;
}

int CammerWidget::getOffsetY()
{
	CIntNode nodeOffsetY = m_pSptrFormatControl->offsetY();
	int64_t nCurVal = 0;
	nodeOffsetY.getValue(nCurVal);
	return nCurVal;
}

double CammerWidget::getShowRate() {
	CDoubleNode frameRate = m_pAcquisitionControl->acquisitionFrameRate();
	double nCurVal = 0;
	frameRate.getValue(nCurVal);
	return nCurVal;
}

double CammerWidget::getShowGain() {
	CDoubleNode nodeGainRaw(m_pCamera, "GainRaw");
	double nCurVal = 0;
	nodeGainRaw.getValue(nCurVal);
	return nCurVal;
}

double CammerWidget::getShowExposure() {
	CDoubleNode nodeExposureTime(m_pCamera, "ExposureTime");
	double nCurVal = 0;
	nodeExposureTime.getValue(nCurVal);
	return nCurVal;
}

// 状态栏统计信息 end 
// Status bar statistics ending