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
	void on_pushButton_Start_clicked();
	void on_pushButton_Stop_clicked();
	void on_pushButton_Open_clicked();
	void on_pushButton_Close_clicked();
	void on_comboBox_currentIndexChanged(int nIndex);
	void closeEvent(QCloseEvent* event);
	void onTimerStreamStatistic();// 定时刷新状态栏信息 | Refresh status bar information regularly

	void on_horizontalSlider_Rote_valueChanged(int value);
	void on_horizontalSlider_Gain_valueChanged(int value);
	void on_pushButton_Resolution_clicked();

	void on_lineEdit_Rote_editingFinished();
	void on_lineEdit_Gain_editingFinished();

	void wheelEvent(QWheelEvent* event);

private:
	Ui::Form* ui;

	Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> m_vCameraPtrList;	// 发现的相机列表 | List of cameras found
	QTimer m_staticTimer;	// 定时器，定时刷新状态栏信息 | Timer, refresh status bar information regularly
};

#endif // FORM_H
