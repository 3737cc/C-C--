#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QTimer>
#include "GenICam/System.h"
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QPointF>

namespace Ui {
	class Form;
}

class Form : public QWidget
{
	Q_OBJECT

public:
	explicit Form(QWidget* parent = 0);
	~Form();
private:
	void initUi();

private slots:
	void on_pushButtonStart_clicked();
	void on_pushButtonStop_clicked();
	void on_pushButtonOpen_clicked();
	void on_pushButtonClose_clicked();
	void on_comboBox_currentIndexChanged(int nIndex);
	void closeEvent(QCloseEvent* event);
	void onTimerStreamStatistic();// 定时刷新状态栏信息 | Refresh status bar information regularly
	//设置帧率
	void on_horizontalSliderRate_valueChanged(int value);
	void on_horizontalSliderGain_valueChanged(int value);
	//设置增益
	void on_lineEditRote_editingFinished();
	void on_lineEditGain_editingFinished();
	//设置曝光
	void on_horizontalSliderExposure_valueChanged(int value);
	void on_lineEditExposure_editingFinished();
	//设置宽度
	void on_horizontalSliderWidth_valueChanged(int value);
	void on_lineEditWidth_editingFinished();
	//设置高度
	void on_horizontalSliderHeight_valueChanged(int value);
	void on_lineEditHeight_editingFinished();
	//设置x偏移量
	void on_horizontalSliderOffsetX_valueChanged(int value);
	void on_lineEditOffsetX_editingFinished();
	//设置y偏移量
	void on_horizontalSliderOffsetY_valueChanged(int value);
	void on_lineEditOffsetY_editingFinished();
	//最大分辨率
	void on_pushButtonMaxResolution_clicked();
	//保存
	void on_pushButtonSave_clicked();

	void on_pushButtonOnestart_clicked();
	void on_pushButtonReset_clicked();
	void on_pushButtonShowProperties_clicked();

	void showImage();
	//动态滑块
	void dynamicSliders();

private:
	Ui::Form* ui;

	Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> m_vCameraPtrList;	// 发现的相机列表 | List of cameras found
	QTimer m_mstaticTimer;	// 定时器，定时刷新状态栏信息 | Timer, refresh status bar information regularly
};

#endif // FORM_H
