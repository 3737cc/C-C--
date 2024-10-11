#pragma once

#include <QMainWindow>
#include "ui_Pipe.h"
#include <QProcess>
#include <QDebug>


class Pipe : public QMainWindow
{
	Q_OBJECT

public:
	Pipe(QWidget* parent = nullptr);
	~Pipe();

private slots:
	void onWriteButtonClicked();
private:
	Ui::PipeClass ui;
	QProcess m_processB;//创建子进程
};
