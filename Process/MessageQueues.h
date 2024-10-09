#pragma once

#include <QMainWindow>
#include "ui_MessageQueues.h"

class MessageQueues : public QMainWindow
{
	Q_OBJECT

public:
	MessageQueues(QWidget *parent = nullptr);
	~MessageQueues();

private:
	Ui::MessageQueuesClass ui;
};
