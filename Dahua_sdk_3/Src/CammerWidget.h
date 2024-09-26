#ifndef CAMMERWIDGET_H
#define CAMMERWIDGET_H

#include <QWidget>
#include "GenICam/System.h"
#include "Media/ImageConvert.h"
#include "MessageQue.h"
#include <QElapsedTimer>
#include <QMutex>
#include "GenICam/ParameterNode.h"
#include "GenICam/AcquisitionControl.h"
#include "GenICam/ImageFormatControl.h"
#include <QTimer>

// 状态栏统计信息 
// Status bar statistics
struct FrameStatInfo
{
	quint32     m_nFrameSize;       // 帧大小, 单位: 字节 | frame size ,length :byte
	qint64      m_nPassTime;         // 接收到该帧时经过的纳秒数 |  The number of nanoseconds passed when the frame was received
	FrameStatInfo(int nSize, qint64 nTime) : m_nFrameSize(nSize), m_nPassTime(nTime)
	{
	}
};

// 帧信息 
// frame imformation
class CFrameInfo : public Dahua::Memory::CBlock
{
public:
	CFrameInfo()
	{
		m_pImageBuf = NULL;
		m_nBufferSize = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_ePixelType = Dahua::GenICam::gvspPixelMono8;
		m_nPaddingX = 0;
		m_nPaddingY = 0;
		m_nTimeStamp = 0;
	}

	~CFrameInfo()
	{
	}

public:
	BYTE* m_pImageBuf;
	int			m_nBufferSize;
	int			m_nWidth;
	int			m_nHeight;
	Dahua::GenICam::EPixelType	m_ePixelType;
	int			m_nPaddingX;
	int			m_nPaddingY;
	uint64_t	m_nTimeStamp;
};

namespace Ui {
	class CammerWidget;
}

class CammerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit CammerWidget(QWidget* parent = 0);
	~CammerWidget();

	// 枚举触发方式
	// Enumeration trigger mode
	enum ETrigType
	{
		trigContinous = 0,	// 连续拉流 | continue grabbing
		trigSoftware = 1,	// 软件触发 | software trigger
		trigLine = 2,		// 外部触发	| external trigger
	};
	enum Mode { Zoom, Crop };//切换模式
	// 打开相机
	// open cmaera
	bool CameraOpen(void);
	// 关闭相机
	// close camera
	bool CameraClose(void);
	// 开始采集
	// start grabbing
	bool CameraStart(void);
	bool CaptureSingleImage(void);
	// 停止采集
	// stop grabbing
	bool CameraStop(void);
	// 取流回调函数
	// get frame callback function
	void FrameCallback(const Dahua::GenICam::CFrame& frame);
	//单帧处理
	void FrameSingle(const CFrame& frame);
	// 切换采集方式、触发方式 （连续采集、外部触发、软件触发）
	// Switch acquisition mode and triggering mode (continuous acquisition, external triggering and software triggering)
	bool CameraChangeTrig(ETrigType trigType = trigContinous);
	// 执行一次软触发        
	// Execute a soft trigger
	bool ExecuteSoftTrig(void);
	// 设置显示频率，默认一秒钟显示30帧
	// Set the display frequency to display 30 frames in one second by default
	void setDisplayFPS(int nFPS);
	// 设置曝光
	// set exposuse time
	bool SetExposeTime(double dExposureTime);
	// 设置增益
	// set gain
	bool SetAdjustPlus(double dGainRaw);
	//更新帧率
	void updateShowRate(double);
	double getShowRate();
	double getShowGain();
	double getShowExposure();
	// 设置当前相机
	// set current camera
	void SetCamera(const QString& strKey);
	//设置宽度
	void setWidth(int);
	int getWidth();
	//设置高度
	void setHeight(int);
	int getHeight();
	//设置X偏移量
	void setOffsetX(int);
	int getOffsetX();
	//设置Y偏移量
	void setOffsetY(int);
	int getOffsetY();
	//设置最大分辨率
	void setMaxResolution();
	//进行重绘
	void setImage(const QImage& newImage);
	//鼠标事件
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event);
	void onCroppingTimerTimeout();
	void mouseReleaseEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
	void wheelEvent(QWheelEvent* event) override;
	//进行缩放
	void scaleImage(float scaleFactor, QPointF mousePos);
	void handleLeftClick(const QPoint& pos);
	void handleRightClick(const QPoint& pos);
	//void startLongPressTimer(const QPoint& pos);
	//void onLongPress();
	//重置图像
	void resetImage();
	// 状态栏统计信息
	// Status bar statistics
	void resetStatistic();
	QString getStatistic();

private:

	// 显示线程
	// display thread 
	void DisplayThreadProc(Dahua::Infra::CThreadLite& lite);

	// 计算该帧是否显示
	// Calculate whether the frame is displayed
	bool isTimeToDisplay();

	// 窗口关闭响应函数
	// Window close response function
	void closeEvent(QCloseEvent* event);

	// 状态栏统计信息
	// Status bar statistics	
	void recvNewFrame(const CFrame& pBuf);
	void updateStatistic();

	//缩放比例和图像偏移
	QRect calculateCropRect();
	QRect applyCrop(const QRect& cropRect);
	void centerImage();
	void alignTopLeft();

private slots:
	// 显示一帧图像
	// display a frame image 
	bool ShowImage(uint8_t* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat);
signals:
	// 显示图像的信号，在displayThread中发送该信号，在主线程中显示
	// Display the signal of the image, send the signal in displaythread, and display it in the main thread
	bool signalShowImage(uint8_t* pRgbFrameBuf, int nWidth, int nHeight, uint64_t nPixelFormat);

private:
	Ui::CammerWidget* ui;

	Dahua::GenICam::ICameraPtr m_pCamera;							// 当前相机 | current camera 
	Dahua::GenICam::IStreamSourcePtr m_pStreamSource;				// 流对象   |  stream object
	Dahua::GenICam::IImageFormatControlPtr m_pSptrFormatControl;	// 图像格式控制指针
	Dahua::GenICam::IAcquisitionControlPtr m_pAcquisitionControl;	// 帧控制指针
	QImage m_aImage;												// 成员变量存储图像
	QRectF m_aTargetRect;											// 目标矩形
	QRectF m_aSourceRect;											// 源矩形
	QPointF m_pImageOffset;											//偏移量
	bool m_bImageSet;												//重置
	float m_fScaleFactor;											//缩放因子
	bool m_bJustResetResolution = true;								//重置缩放因子和偏移量
	float m_fMinScaleFactor;										//最小缩放因子
	QPoint m_startPoint;											//鼠标开始坐标
	QPoint m_endPoint;												//鼠标结束坐标
	bool m_isCropping;												//鼠标移动标志
	QRect cropRect;													//裁剪框
	QTimer m_croppingTimer;											//定时器
	bool m_isScaling = false;
	QPointF m_lastMousePos;

	QTimer* m_longPressTimer;
	QPoint m_lastClickPos;
	bool m_isLongPress = false;

	Dahua::Infra::CThreadLite           m_thdDisplayThread;			// 显示线程      | diaplay thread 
	TMessageQue<CFrameInfo>				m_qDisplayFrameQueue;		// 显示队列      | diaplay queue

	// 控制显示帧率   | Control display frame rate
	Dahua::Infra::CMutex				m_mxTime;
	int									m_nDisplayInterval;         // 显示间隔 | diaplay time internal
	uintmax_t							m_nFirstFrameTime;          // 第一帧的时间戳 | Time stamp of the first frame
	uintmax_t							m_nLastFrameTime;           // 上一帧的时间戳 | Timestamp of previous frame
	QElapsedTimer						m_elapsedTimer;				// 用来计时，控制显示帧率 | Used to time and control the display frame rate

	// 状态栏统计信息 
	// Status bar statistics	
	typedef std::list<FrameStatInfo> FrameList;
	FrameList   m_listFrameStatInfo;
	QMutex      m_mxStatistic;
	quint64     m_nTotalFrameCount;		// 收到的总帧数 | recieve all frames
	QString     m_szStatistic;			// 统计信息, 不包括错误  | Statistics, excluding errors
	bool		m_bNeedUpdate;
};

#endif // CAMMERWIDGET_H