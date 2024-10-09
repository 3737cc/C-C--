#include "messagequeues.h"

MessageQueues::MessageQueues(QWidget* parent)
	: QMainWindow(parent), messageQueue(new MessageQueue)
{
	ui.setupUi(this);

	// 连接按钮信号与槽函数
	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueues::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueues::onReadButtonClicked);
}

// 写入消息到队列
void MessageQueues::onWriteButtonClicked()
{
	QString inputText = ui.valueInput->text();
	if (inputText.isEmpty()) {
		QMessageBox::warning(this, "Warning", "Input cannot be empty!");
		return;
	}

	// 创建消息并放入队列
	Message message(1, inputText.toStdString()); // 假设每个消息的ID是1，实际可以动态分配
	messageQueue->push(message);

	// 清空输入框
	ui.valueInput->clear();
}

// 从队列中读取消息
void MessageQueues::onReadButtonClicked()
{
	if (messageQueue->empty()) {
		QMessageBox::information(this, "Information", "No messages in queue!");
		return;
	}

	// 从队列中读取消息
	Message message = messageQueue->pop();

	// 显示读取到的消息
	QString outputText = QString::fromStdString(message.body);
	ui.valueOutput->setText(outputText);

	// 更新表格数据 (可视化队列状态)
	updateTable(message);
}

// 更新表格显示消息内容
void MessageQueues::updateTable(const Message& message)
{
	int currentRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(currentRow);

	// 插入消息ID
	QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(message.id));
	ui.memoryBlockTable->setItem(currentRow, 0, idItem);

	// 插入消息内容
	QTableWidgetItem* bodyItem = new QTableWidgetItem(QString::fromStdString(message.body));
	ui.memoryBlockTable->setItem(currentRow, 1, bodyItem);

	// 插入一些虚拟数据作为示例
	QTableWidgetItem* dummyItem1 = new QTableWidgetItem("Address X");
	ui.memoryBlockTable->setItem(currentRow, 2, dummyItem1);

	QTableWidgetItem* dummyItem2 = new QTableWidgetItem("Status Y");
	ui.memoryBlockTable->setItem(currentRow, 3, dummyItem2);
}
