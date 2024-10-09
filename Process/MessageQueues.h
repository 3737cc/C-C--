#include "message.h"
#include "ui_messagequeues.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>

class MessageQueues : public QMainWindow
{
	Q_OBJECT

public:
	MessageQueues(QWidget* parent = nullptr);

private slots:
	void onWriteButtonClicked();
	void onReadButtonClicked();

private:
	void updateTable(const Message& message);

private:
	Ui::MessageQueuesClass ui;
	MessageQueue* messageQueue;
};
