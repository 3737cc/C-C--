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
	void on_horizontalSliderRote_valueChanged(int value);
	void on_horizontalSliderGain_valueChanged(int value);
	void on_pushButtonResolution_clicked();
	void on_lineEditRote_editingFinished();
	void on_lineEditGain_editingFinished();
	void wheelEvent(QWheelEvent* event);
	void on_horizontalSliderExposure_valueChanged(int value);
	void on_lineEditExposure_editingFinished();
	void on_pushButtonOnestart_clicked();

private:
	Ui::Form* ui;

	Dahua::Infra::TVector<Dahua::GenICam::ICameraPtr> mvCameraPtrList;	// 发现的相机列表 | List of cameras found
	QTimer mstaticTimer;	// 定时器，定时刷新状态栏信息 | Timer, refresh status bar information regularly
	double zoomFactor;
};

#endif // FORM_H
