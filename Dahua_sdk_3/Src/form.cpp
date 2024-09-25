#include "form.h"
#include "ui_form.h"
#include "qdebug.h"

Form::Form(QWidget* parent) :
	QWidget(parent)
	, ui(new Ui::Form)
{
	ui->setupUi(this);

	connect(&m_mstaticTimer, &QTimer::timeout, this, &Form::onTimerStreamStatistic);

	initUi();
}

Form::~Form()
{
	delete ui;
}

void Form::initUi()
{
	ui->horizontalSliderRate->setRange(0, 2000);
	ui->horizontalSliderGain->setRange(0, 200);
	ui->horizontalSliderExposure->setRange(0, 2000);
	ui->horizontalSliderWidth->setRange(0, 2592);
	ui->horizontalSliderHeight->setRange(0, 2048);
	ui->horizontalSliderOffsetX->setRange(0, 2592);
	ui->horizontalSliderOffsetY->setRange(0, 2048);

	ui->labelStatistic->setText("");// 连接相机之前不显示状态栏 | Don't show status bar before connecting camera

	CSystem& systemObj = CSystem::getInstance();
	if (false == systemObj.discovery(m_vCameraPtrList))
	{
		printf("discovery fail.\n");
		return;
	}
	if (m_vCameraPtrList.size() < 1)
	{
		ui->comboBox->setEnabled(false);
		ui->pushButtonOpen->setEnabled(false);
	}
	else
	{
		ui->comboBox->setEnabled(true);
		ui->pushButtonOpen->setEnabled(true);

		for (int i = 0; i < m_vCameraPtrList.size(); i++)
		{
			ui->comboBox->addItem(m_vCameraPtrList[i]->getKey());
		}

		ui->widget->SetCamera(m_vCameraPtrList[0]->getKey());
	}

	ui->pushButtonClose->setEnabled(false);
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonStop->setEnabled(false);
	ui->pushButtonOnestart->setEnabled(false);
	ui->pushButtonReset->setEnabled(false);
}

// 设置要连接的相机
// set camera which need to connect
void Form::on_comboBox_currentIndexChanged(int nIndex)
{
	ui->widget->SetCamera(m_vCameraPtrList[nIndex]->getKey());
}

// 连接
// connect
void Form::on_pushButtonOpen_clicked()
{
	if (!ui->widget->CameraOpen())
	{
		return;
	}

	ui->pushButtonOpen->setEnabled(false);
	ui->pushButtonClose->setEnabled(true);
	ui->pushButtonStart->setEnabled(true);
	ui->pushButtonOnestart->setEnabled(true);
	ui->pushButtonStop->setEnabled(false);
	ui->comboBox->setEnabled(false);
	ui->pushButtonReset->setEnabled(false);

	//显示相机内设参数
	showImage();
	// 连接相机之后显示统计信息，所有值为0
	// Show statistics after connecting camera, all values are 0
	ui->widget->resetStatistic();
	QString strStatic = ui->widget->getStatistic();
	ui->labelStatistic->setText(strStatic);
}

// 断开
// disconnect
void Form::on_pushButtonClose_clicked()
{
	on_pushButtonStop_clicked();
	ui->widget->CameraClose();
	ui->labelStatistic->setText("");// 断开相机以后不显示状态栏 | Do not display the status bar after disconnecting the camera
	ui->pushButtonOpen->setEnabled(true);
	ui->pushButtonClose->setEnabled(false);
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonOnestart->setEnabled(false);
	ui->pushButtonStop->setEnabled(false);
	ui->comboBox->setEnabled(true);
	ui->pushButtonReset->setEnabled(false);
}

// 开始
// start 
void Form::on_pushButtonStart_clicked()
{
	// 设置连续拉流
	// set continue grabbing
	// ui->widget->CameraChangeTrig(CammerWidget::trigContinous);

	ui->widget->CameraStart();
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonStop->setEnabled(true);
	ui->pushButtonOnestart->setEnabled(false);
	ui->pushButtonReset->setEnabled(true);
	ui->widget->resetStatistic();
	m_mstaticTimer.start(100);

	showImage();
}

void Form::on_pushButtonOnestart_clicked()
{
	ui->widget->CaptureSingleImage();
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonStop->setEnabled(true);
	ui->pushButtonOnestart->setEnabled(false);
	ui->pushButtonReset->setEnabled(true);
	ui->widget->resetStatistic();
	m_mstaticTimer.start(100);
}

// 停止
// stop
void Form::on_pushButtonStop_clicked()
{
	m_mstaticTimer.stop();
	ui->widget->CameraStop();
	ui->pushButtonStart->setEnabled(true);
	ui->pushButtonStop->setEnabled(false);
	ui->pushButtonOnestart->setEnabled(true);
	ui->pushButtonReset->setEnabled(false);
}

void Form::onTimerStreamStatistic()
{
	QString strStatic = ui->widget->getStatistic();
	ui->labelStatistic->setText(strStatic);
}

void Form::closeEvent(QCloseEvent* event)
{
	on_pushButtonStop_clicked();
	ui->widget->CameraClose();
}

//设置帧率
void Form::on_horizontalSliderRate_valueChanged(int value)
{
	ui->widget->updateShowRate(value);
	ui->lineEditRate->setText(QString::number(value));
}

void Form::on_lineEditRote_editingFinished()
{
	QString roteText = ui->lineEditRate->text();
	bool roteOk;
	int onRote = roteText.toInt(&roteOk);

	if (roteOk)
	{
		ui->horizontalSliderRate->setValue(onRote);
		ui->widget->updateShowRate(onRote);
	}
}

//设置增益
void Form::on_horizontalSliderGain_valueChanged(int value)
{
	ui->widget->SetAdjustPlus(value);
	ui->lineEditGain->setText(QString::number(value));
}

void Form::on_lineEditGain_editingFinished()
{
	QString gainText = ui->lineEditGain->text();
	bool gainOk;
	int onGain = gainText.toInt(&gainOk);

	if (gainOk)
	{
		ui->horizontalSliderGain->setValue(onGain);
		ui->widget->SetAdjustPlus(onGain);
	}
}

//设置曝光
void Form::on_horizontalSliderExposure_valueChanged(int value)
{
	ui->widget->SetExposeTime(value);
	ui->lineEditExposure->setText(QString::number(value));
}

void Form::on_lineEditExposure_editingFinished()
{
	QString exposureText = ui->lineEditExposure->text();
	bool exposureOk;
	int onExposure = exposureText.toInt(&exposureOk);

	if (exposureOk)
	{
		ui->horizontalSliderExposure->setValue(onExposure);
		ui->widget->SetExposeTime(onExposure);
	}
}

//设置宽度
void Form::on_horizontalSliderWidth_valueChanged(int value)
{
	ui->widget->setWidth(value);
	ui->lineEditWidth->setText(QString::number(value));
}

void Form::on_lineEditWidth_editingFinished()
{
	QString exposureText = ui->lineEditWidth->text();
	bool exposureOk;
	int onExposure = exposureText.toInt(&exposureOk);

	if (exposureOk)
	{
		ui->horizontalSliderWidth->setValue(onExposure);
		ui->widget->setWidth(onExposure);
	}
}

//设置高度
void Form::on_horizontalSliderHeight_valueChanged(int value)
{
	ui->widget->setHeight(value);
	ui->lineEditHeight->setText(QString::number(value));
}

void Form::on_lineEditHeight_editingFinished()
{
	QString exposureText = ui->lineEditHeight->text();
	bool exposureOk;
	int onExposure = exposureText.toInt(&exposureOk);

	if (exposureOk)
	{
		ui->horizontalSliderHeight->setValue(onExposure);
		ui->widget->setHeight(onExposure);
	}
}

//设置X偏移量
void Form::on_horizontalSliderOffsetX_valueChanged(int value)
{
	ui->widget->setOffsetX(value);
	ui->lineEditOffsetX->setText(QString::number(value));
}

void Form::on_lineEditOffsetX_editingFinished()
{
	QString exposureText = ui->lineEditOffsetX->text();
	bool exposureOk;
	int onExposure = exposureText.toInt(&exposureOk);

	if (exposureOk)
	{
		ui->horizontalSliderOffsetX->setValue(onExposure);
		ui->widget->setOffsetX(onExposure);
	}
}

//设置Y偏移量
void Form::on_horizontalSliderOffsetY_valueChanged(int value)
{
	ui->widget->setOffsetY(value);
	ui->lineEditOffsetY->setText(QString::number(value));
}

void Form::on_lineEditOffsetY_editingFinished()
{
	QString exposureText = ui->lineEditOffsetY->text();
	bool exposureOk;
	int onExposure = exposureText.toInt(&exposureOk);

	if (exposureOk)
	{
		ui->horizontalSliderOffsetY->setValue(onExposure);
		ui->widget->setOffsetY(onExposure);
	}
}

//最大分辨率
void Form::on_pushButtonMaxResolution_clicked()
{
	ui->widget->setMaxResolution();
}

//复位图像
void Form::on_pushButtonReset_clicked()
{
	ui->widget->resetImage();
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonStop->setEnabled(true);
	ui->pushButtonOnestart->setEnabled(false);
	showImage();
}

//框选裁剪图像
void Form::on_pushButtonCropImage_clicked() {
	// 切换到裁剪模式
	ui->widget->setCurrentMode();
	// 更新界面
	ui->widget->update(); // 确保界面重绘
	showImage();
}

void Form::showImage() {
	double l_iRate = ui->widget->getShowRate();
	double l_iGain = ui->widget->getShowGain();
	double l_iExposure = ui->widget->getShowExposure();
	int64_t l_iWidth = ui->widget->getWidth();
	int64_t l_iHeight = ui->widget->getHeight();
	int64_t l_iOffsetX = ui->widget->getOffsetX();
	int64_t l_iOffsetY = ui->widget->getOffsetY();

	ui->horizontalSliderRate->setValue(l_iRate);
	ui->horizontalSliderGain->setValue(l_iGain);
	ui->horizontalSliderExposure->setValue(l_iExposure);
	ui->horizontalSliderWidth->setValue(l_iWidth);
	ui->horizontalSliderHeight->setValue(l_iHeight);
	ui->horizontalSliderOffsetX->setValue(l_iOffsetX);
	ui->horizontalSliderOffsetY->setValue(l_iOffsetY);

}