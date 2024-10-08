#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Process.h"
#include "SharedMemory.h"

class Process : public QMainWindow
{
	Q_OBJECT

public:
	Process(QWidget* parent = nullptr);
	~Process();

private slots:
	void onReadButtonClicked();
	void onWriteButtonClicked();

private:
	Ui::Process ui;
	SharedMemory sharedMemory;// 共享内存对象
};
