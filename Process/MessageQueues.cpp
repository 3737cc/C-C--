#include "messagequeues.h"

MessageQueues::MessageQueues(QWidget* parent)
	: QMainWindow(parent), messageQueue(new MessageQueue)
{
	ui.setupUi(this);

	// ���Ӱ�ť�ź���ۺ���
	connect(ui.writeButton, &QPushButton::clicked, this, &MessageQueues::onWriteButtonClicked);
	connect(ui.readButton, &QPushButton::clicked, this, &MessageQueues::onReadButtonClicked);
}

// д����Ϣ������
void MessageQueues::onWriteButtonClicked()
{
	QString inputText = ui.valueInput->text();
	if (inputText.isEmpty()) {
		QMessageBox::warning(this, "Warning", "Input cannot be empty!");
		return;
	}

	// ������Ϣ���������
	Message message(1, inputText.toStdString()); // ����ÿ����Ϣ��ID��1��ʵ�ʿ��Զ�̬����
	messageQueue->push(message);

	// ��������
	ui.valueInput->clear();
}

// �Ӷ����ж�ȡ��Ϣ
void MessageQueues::onReadButtonClicked()
{
	if (messageQueue->empty()) {
		QMessageBox::information(this, "Information", "No messages in queue!");
		return;
	}

	// �Ӷ����ж�ȡ��Ϣ
	Message message = messageQueue->pop();

	// ��ʾ��ȡ������Ϣ
	QString outputText = QString::fromStdString(message.body);
	ui.valueOutput->setText(outputText);

	// ���±������ (���ӻ�����״̬)
	updateTable(message);
}

// ���±����ʾ��Ϣ����
void MessageQueues::updateTable(const Message& message)
{
	int currentRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(currentRow);

	// ������ϢID
	QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(message.id));
	ui.memoryBlockTable->setItem(currentRow, 0, idItem);

	// ������Ϣ����
	QTableWidgetItem* bodyItem = new QTableWidgetItem(QString::fromStdString(message.body));
	ui.memoryBlockTable->setItem(currentRow, 1, bodyItem);

	// ����һЩ����������Ϊʾ��
	QTableWidgetItem* dummyItem1 = new QTableWidgetItem("Address X");
	ui.memoryBlockTable->setItem(currentRow, 2, dummyItem1);

	QTableWidgetItem* dummyItem2 = new QTableWidgetItem("Status Y");
	ui.memoryBlockTable->setItem(currentRow, 3, dummyItem2);
}
