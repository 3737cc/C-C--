#include "ProcessB.h"

ProcessB::ProcessB(QWidget* parent)
	: QMainWindow(parent)
	, sharedMemory("MySharedMemoryKey") // 初始化共享内存，使用相同的键
{
	ui.setupUi(this);

	QObject::connect(ui.readButton, &QPushButton::clicked, this, &ProcessB::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &ProcessB::onWriteButtonClicked);

	// 连接共享内存
	if (!sharedMemory.Attach()) {
		qDebug() << "Unable to connect to shared memory.";
	}
}

ProcessB::~ProcessB()
{

}

void ProcessB::onReadButtonClicked() {
	QString data = sharedMemory.Read(); // 从共享内存读取数据
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
