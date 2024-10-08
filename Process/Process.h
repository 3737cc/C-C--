#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Process.h"
#include "SharedMemory.h"
#include <QMainWindow>
#include <QTimer>
#include <QProgressBar>
#include <QPushButton>
#include <QDebug>
#include <QSharedMemory>
#include <windows.h>

class Process : public QMainWindow
{
	Q_OBJECT

public:
	Process(QWidget* parent = nullptr);

private slots:
	void onSizeButtonClicked();
	void onReadButtonClicked();
	void onWriteButtonClicked();
	void updateMemoryUsage();

private:
	double getMemoryUsage();

private:
	Ui::Process ui;
	SharedMemory m_sharedMemory;// 共享内存对象
	QProgressBar* memoryUsageBar;
	QTimer* m_timer;          // 定时器对象
};
