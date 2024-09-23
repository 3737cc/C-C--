#include "form.h"
#include "ui_form.h"

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
	ui->horizontalSliderRote->setRange(0, 2000);
	ui->horizontalSliderGain->setRange(0, 200);
	ui->horizontalSliderExposure->setRange(0, 2000);

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
void Form::on_horizontalSliderRote_valueChanged(int value)
{
	ui->widget->updateShowRate(value);
	ui->lineEditRote->setText(QString::number(value));
}

void Form::on_lineEditRote_editingFinished()
{
	QString roteText = ui->lineEditRote->text();
	bool roteOk;
	int onRote = roteText.toInt(&roteOk);

	if (roteOk)
	{
		ui->horizontalSliderRote->setValue(onRote);
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

//设置分辨率
void Form::on_pushButtonResolution_clicked()
{
	QString widthText = ui->lineEditWidth->text();
	QString heightText = ui->lineEditHeight->text();

	// 将文本转换为整数
	bool widthOk, heightOk;
	int on_Width = widthText.toInt(&widthOk);
	int on_Height = heightText.toInt(&heightOk);

	ui->widget->resolution(on_Width, on_Height);
}

//复位图像
void Form::on_pushButtonReset_clicked()
{
	ui->widget->resetImage();
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonStop->setEnabled(true);
	ui->pushButtonOnestart->setEnabled(false);
}

