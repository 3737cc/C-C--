#include "ProcessB.h"

ProcessB::ProcessB(QWidget* parent)
	: QMainWindow(parent)
	, sharedMemory("MySharedMemoryKey") // ��ʼ�������ڴ棬ʹ����ͬ�ļ�
{
	ui.setupUi(this);

	QObject::connect(ui.readButton, &QPushButton::clicked, this, &ProcessB::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &ProcessB::onWriteButtonClicked);

	// ���ӹ����ڴ�
	if (!sharedMemory.Attach()) {
		qDebug() << "Unable to connect to shared memory.";
	}
}

ProcessB::~ProcessB()
{

}

void ProcessB::onReadButtonClicked() {
	QString data = sharedMemory.Read(); // �ӹ����ڴ��ȡ����
	if (!data.isEmpty()) {
		qDebug() << "B:Data read from shared memory:" << data;
		ui.addressInput->setText(data);
	}
	else {
		qDebug() << "B:No data found in shared memory.";
	}
}

void ProcessB::onWriteButtonClicked() {
	QString dataToWrite = ui.valueInput->text();
	if (sharedMemory.Write(dataToWrite)) {
		qDebug() << "B:Data written to shared memory:" << dataToWrite;
	}
	else {
		qDebug() << "B:Failed to write data to shared memory.";
	}

}
