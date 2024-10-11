#pragma once

#include <QMainWindow>
#include "ui_PipeB.h"
#include <QProcess>

class PipeB : public QMainWindow
{
	Q_OBJECT

public:
	PipeB(QWidget* parent = nullptr);
	~PipeB();

public slots:
	void onReadButtonClicked();

private:
	Ui::PipeBClass ui;
	QProcess* m_process;
};
