#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ProcessB.h"
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
#include <QMessageBox>
#include "Process.h"

class ProcessB : public QMainWindow
{
	Q_OBJECT

public:
	ProcessB(QWidget* parent = nullptr);

private slots:
	//void onSizeButtonClicked();
	void onReadButtonClicked();
	void onWriteButtonClicked();
private:
	double getMemoryUsage();
	QString generateRandomString(int length);

private:
	Ui::ProcessBClass ui;
	SharedMemory m_sharedMemory;// 共享内存对象
	QProgressBar* memoryUsageBar;
	const int m_iMaxSharedMemory;
	QTimer* m_timer;          // 定时器对象
	LARGE_INTEGER m_frequency; // 计时器频率
	LARGE_INTEGER m_start, m_end; // 计时器值

	const int m_iByte;//随机发送消息大小
	const int m_iNumberOfEntries;//生成多少随机数据
	const int l_iTestRuns;  // 进行多次测试
	//列表值更新
	QTableWidgetItem* m_shared;
	QTableWidgetItem* m_numberOfEntries;
	QTableWidgetItem* m_byte;
	QTableWidgetItem* m_time;
};
