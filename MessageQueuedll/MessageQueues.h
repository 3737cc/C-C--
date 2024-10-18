// messagequeues.h
#pragma once

#include "ui_messagequeues.h"
#include <QMainWindow>
#include <MessageQueueManager.h>
#include <iostream>
#include <Windows.h>
#include <QMessageBox>
#include <random>
#include <QTimer>

class MessageQueues : public QMainWindow
{
	Q_OBJECT
public:
	MessageQueues(QWidget* parent = nullptr);
	~MessageQueues();

private slots:
	void onWriteButtonClicked();
	void onReadButtonClicked();
	//void startAutoSending();
	//void autoSendMessage();
	//void onStartAutoSendingClicked();
	//void startAutoReading();
	//void autoReadMessage();
	//void onStartAutoReadingClicked();

	std::string generateRandomSring(size_t length);

private:
	Ui::MessageQueuesClass ui;
	MessageQueueManager m_queueManager;

	// 性能计数器变量
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_end;

	// 表格项
	QTableWidgetItem* m_queue;
	QTableWidgetItem* m_byte;
	QTableWidgetItem* m_time;

	// 配置参数
	const int m_iByte;
	const int m_iNumQM;
	const int m_iMaxByte;
	const int m_iMaxQueueMessages;
};