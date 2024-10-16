#pragma once

#include <QMainWindow>
#include "ui_ProcessB.h"
#include "SharedMemory.h"

class ProcessB : public QMainWindow
{
	Q_OBJECT

public:
	ProcessB(QWidget* parent = nullptr);
	~ProcessB();

	void onReadButtonClicked();
	void onWriteButtonClicked();

private:
	Ui::ProcessBClass ui;
	SharedMemory m_sharedMemory;// 共享内存对象
};
