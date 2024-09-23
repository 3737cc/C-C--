#include "form.h"
#include "ui_form.h"

Form::Form(QWidget* parent) :
	QWidget(parent)
	, ui(new Ui::Form)
	, zoomFactor(1.0)
{
	ui->setupUi(this);

	connect(&m_staticTimer, &QTimer::timeout, this, &Form::onTimerStreamStatistic);

	initUi();
}

Form::~Form()
{
	delete ui;
}

void Form::initUi()
{
	ui->horizontalSlider_Rote->setRange(0, 2000);
	ui->horizontalSlider_Gain->setRange(0, 200);
	ui->horizontalSlider_Exposure->setRange(0, 2000);

	ui->label_Statistic->setText("");// 连接相机之前不显示状态栏 | Don't show status bar before connecting camera

	CSystem& systemObj = CSystem::getInstance();
	if (false == systemObj.discovery(m_vCameraPtrList))
	{
		printf("discovery fail.\n");
		return;
	}
	if (m_vCameraPtrList.size() < 1)
	{
		ui->comboBox->setEnabled(false);
		ui->pushButton_Open->setEnabled(false);
	}
	else
	{
		ui->comboBox->setEnabled(true);
		ui->pushButton_Open->setEnabled(true);

		for (int i = 0; i < m_vCameraPtrList.size(); i++)
		{
			ui->comboBox->addItem(m_vCameraPtrList[i]->getKey());
		}

		ui->widget->SetCamera(m_vCameraPtrList[0]->getKey());
	}

	ui->pushButton_Close->setEnabled(false);
	ui->pushButton_Start->setEnabled(false);
	ui->pushButton_Stop->setEnabled(false);
	ui->pushButton_Onestart->setEnabled(false);

}

// 设置要连接的相机
// set camera which need to connect
void Form::on_comboBox_currentIndexChanged(int nIndex)
{
	ui->widget->SetCamera(m_vCameraPtrList[nIndex]->getKey());
}

// 连接
// connect
void Form::on_pushButton_Open_clicked()
{
	if (!ui->widget->CameraOpen())
	{
		return;
	}

	ui->pushButton_Open->setEnabled(false);
	ui->pushButton_Close->setEnabled(true);
	ui->pushButton_Start->setEnabled(true);
	ui->pushButton_Onestart->setEnabled(true);
	ui->pushButton_Stop->setEnabled(false);
	ui->comboBox->setEnabled(false);

	// 连接相机之后显示统计信息，所有值为0
	// Show statistics after connecting camera, all values are 0
	ui->widget->resetStatistic();
	QString strStatic = ui->widget->getStatistic();
	ui->label_Statistic->setText(strStatic);
}

// 断开
// disconnect
void Form::on_pushButton_Close_clicked()
{
	on_pushButton_Stop_clicked();
	ui->widget->CameraClose();

	ui->label_Statistic->setText("");// 断开相机以后不显示状态栏 | Do not display the status bar after disconnecting the camera

	ui->pushButton_Open->setEnabled(true);
	ui->pushButton_Close->setEnabled(false);
	ui->pushButton_Start->setEnabled(false);
	ui->pushButton_Onestart->setEnabled(false);
	ui->pushButton_Stop->setEnabled(false);
	ui->comboBox->setEnabled(true);
}

// 开始
// start 
void Form::on_pushButton_Start_clicked()
{
	// 设置连续拉流
	// set continue grabbing
	// ui->widget->CameraChangeTrig(CammerWidget::trigContinous);

	ui->widget->CameraStart();

	ui->pushButton_Start->setEnabled(false);
	ui->pushButton_Stop->setEnabled(true);
	ui->pushButton_Onestart->setEnabled(false);

	ui->widget->resetStatistic();
	m_staticTimer.start(100);
}

void Form::on_pushButton_Onestart_clicked()
{
	ui->widget->CaptureSingleImage();

	ui->pushButton_Start->setEnabled(false);
	ui->pushButton_Stop->setEnabled(true);
	ui->pushButton_Onestart->setEnabled(false);

	ui->widget->resetStatistic();
	m_staticTimer.start(100);
}

// 停止
// stop
void Form::on_pushButton_Stop_clicked()
{
	m_staticTimer.stop();

	ui->widget->CameraStop();

	ui->pushButton_Start->setEnabled(true);
	ui->pushButton_Stop->setEnabled(false);
	ui->pushButton_Onestart->setEnabled(true);
}

void Form::onTimerStreamStatistic()
{
	QString strStatic = ui->widget->getStatistic();
	ui->label_Statistic->setText(strStatic);
}

void Form::closeEvent(QCloseEvent* event)
{
	on_pushButton_Stop_clicked();
	ui->widget->CameraClose();
}

//设置帧率
void Form::on_horizontalSlider_Rote_valueChanged(int value)
{
	ui->widget->updateShowRate(value);
	ui->lineEdit_Rote->setText(QString::number(value));
}

void Form::on_lineEdit_Rote_editingFinished()
{
	QString roteText = ui->lineEdit_Rote->text();
	bool roteOk;
	int onRote = roteText.toInt(&roteOk);

	if (roteOk)
	{
		ui->horizontalSlider_Rote->setValue(onRote);
		ui->widget->updateShowRate(onRote);
	}
}

//设置增益
void Form::on_horizontalSlider_Gain_valueChanged(int value)
{
	ui->widget->SetAdjustPlus(value);
	ui->lineEdit_Gain->setText(QString::number(value));
}

void Form::on_lineEdit_Gain_editingFinished()
{
	QString gainText = ui->lineEdit_Gain->text();
	bool gainOk;
	int onGain = gainText.toInt(&gainOk);

	if (gainOk)
	{
		ui->horizontalSlider_Gain->setValue(onGain);
		ui->widget->SetAdjustPlus(onGain);
	}
}

//设置曝光
void Form::on_horizontalSlider_Exposure_valueChanged(int value)
{
	ui->widget->SetExposeTime(value);
	ui->lineEdit_Exposure->setText(QString::number(value));
}

void Form::on_lineEdit_Exposure_editingFinished()
{
	QString exposureText = ui->lineEdit_Exposure->text();
	bool exposureOk;
	int onExposure = exposureText.toInt(&exposureOk);

	if (exposureOk)
	{
		ui->horizontalSlider_Exposure->setValue(onExposure);
		ui->widget->SetExposeTime(onExposure);
	}
}

//设置分辨率
void Form::on_pushButton_Resolution_clicked()
{
	QString widthText = ui->lineEdit_Width->text();
	QString heightText = ui->lineEdit_Height->text();

	// 将文本转换为整数（如果你有宽度和高度的输入框）
	bool widthOk, heightOk;
	int on_Width = widthText.toInt(&widthOk);
	int on_Height = heightText.toInt(&heightOk);

	ui->widget->resolution(on_Width, on_Height);
}

//显示的放大与缩小
void Form::wheelEvent(QWheelEvent* event) {
	int delta = event->angleDelta().y(); // 获取滚轮的增量
	if (delta > 0) {
		zoomFactor *= 1.1;
		ui->widget->zoomIn(); // 放大
	}
	else {
		zoomFactor /= 1.1;
		ui->widget->zoomOut(); // 缩小
	}
	event->accept();
}
