#pragma once

#include <QMainWindow>
#include "ui_Semaphore.h"

class Semaphore : public QMainWindow
{
	Q_OBJECT

public:
	Semaphore(QWidget *parent = nullptr);
	~Semaphore();

private:
	Ui::SemaphoreClass ui;
};
