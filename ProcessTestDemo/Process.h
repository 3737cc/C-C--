#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Process.h"
#include "SharedMemory.h"
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <windows.h>
#include <iostream>
#include <QMessageBox>

extern int g_iSharedMemoryMessages;

class Process : public QMainWindow
{
	Q_OBJECT

public:
	Process(QWidget* parent = nullptr);

private slots:
	void onReadButtonClicked();
	void onWriteButtonClicked();

private:
	Ui::Process ui;
	SharedMemory m_sharedMemory;// 共享内存对象
	QProgressBar* memoryUsageBar;
	const int m_iMaxSharedMemory;
	QTimer* m_timer;          // 定时器对象
	LARGE_INTEGER m_frequency; // 计时器频率
	LARGE_INTEGER m_start, m_end; // 计时器值

	const int m_iTestRuns;  // 进行多次测试
	//列表值更新
	QTableWidgetItem* m_shared;
	QTableWidgetItem* m_numberOfEntries;
	QTableWidgetItem* m_byte;
	QTableWidgetItem* m_time;
};
