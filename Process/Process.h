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
#include <iostream>
#include <random>

class Process : public QMainWindow
{
	Q_OBJECT

public:
	Process(QWidget* parent = nullptr);

private slots:
	//void onSizeButtonClicked();
	void onReadButtonClicked();
	void onWriteButtonClicked();
	void updateMemoryUsage();

private:
	double getMemoryUsage();
	QString generateRandomString(int length);

private:
	Ui::Process ui;
	SharedMemory m_sharedMemory;// 共享内存对象
	QProgressBar* memoryUsageBar;
	QTimer* m_timer;          // 定时器对象
	LARGE_INTEGER m_frequency; // 计时器频率
	LARGE_INTEGER m_start, m_end; // 计时器值
};
