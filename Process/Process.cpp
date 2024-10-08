#include "Process.h"

Process::Process(QWidget* parent)
	: QMainWindow(parent)
	, sharedMemory("MySharedMemoryKey") // 初始化共享内存，使用相同的键
{
	ui.setupUi(this);

	QObject::connect(ui.readButton, &QPushButton::clicked, this, &Process::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &Process::onWriteButtonClicked);

	// 创建共享内存
	if (!sharedMemory.Create(1024)) {
		qDebug() << "Failed to create shared memory.";
	}
}

Process::~Process()
{

}

void Process::onReadButtonClicked() {
	QString data = sharedMemory.Read(); // 从共享内存读取数据
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
