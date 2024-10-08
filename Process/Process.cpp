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
		qDebug() << "A:Data read from shared memory:" << data;
		ui.valueOutput->setText(data);

		// 假设你的共享内存类有方法获取地址、大小和状态
		void* address = sharedMemory.getAddress(); // 获取共享内存地址
		size_t size = sharedMemory.getSize();      // 获取共享内存大小
		QString status = sharedMemory.getStatus();  // 获取共享内存状态

		// 在memoryBlockTable中添加行
		int rowCount = ui.memoryBlockTable->rowCount();
		ui.memoryBlockTable->insertRow(rowCount); // 在最后插入新行

		// 将数据填入新行
		ui.memoryBlockTable->setItem(rowCount, 0, new QTableWidgetItem(QString::number(reinterpret_cast<quintptr>(address)))); // 地址
		ui.memoryBlockTable->setItem(rowCount, 1, new QTableWidgetItem(QString::number(size))); // 大小
		ui.memoryBlockTable->setItem(rowCount, 2, new QTableWidgetItem(status)); // 状态
	}
	else {
		qDebug() << "A:No data found in shared memory.";
	}
}

void Process::onWriteButtonClicked() {
	QString dataToWrite = ui.valueInput->text();
	if (sharedMemory.Write(dataToWrite)) {
		qDebug() << "A:Data written to shared memory:" << dataToWrite;
	}
	else {
		qDebug() << "A:Failed to write data to shared memory.";
	}

}
