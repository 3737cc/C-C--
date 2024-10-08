#include "Process.h"

Process::Process(QWidget* parent)
	: QMainWindow(parent)
	, sharedMemory("MySharedMemoryKey") // ��ʼ�������ڴ棬ʹ����ͬ�ļ�
{
	ui.setupUi(this);

	QObject::connect(ui.readButton, &QPushButton::clicked, this, &Process::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &Process::onWriteButtonClicked);

	// ���������ڴ�
	if (!sharedMemory.Create(1024)) {
		qDebug() << "Failed to create shared memory.";
	}
}

Process::~Process()
{

}

void Process::onReadButtonClicked() {
	QString data = sharedMemory.Read(); // �ӹ����ڴ��ȡ����
	if (!data.isEmpty()) {
		qDebug() << "Data read from shared memory:" << data;
		ui.addressInput->setText(data);
	}
	else {
		qDebug() << "No data found in shared memory.";
	}
}

void Process::onWriteButtonClicked() {
	QString dataToWrite = ui.valueInput->text();
	if (sharedMemory.Write(dataToWrite)) {
		qDebug() << "Data written to shared memory:" << dataToWrite;
	}
	else {
		qDebug() << "Failed to write data to shared memory.";
	}

}
