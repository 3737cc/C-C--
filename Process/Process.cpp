#include "Process.h"

Process::Process(QWidget* parent)
	: QMainWindow(parent)
	, m_sharedMemory("MySharedMemoryKey") // 初始化共享内存，使用相同的键
{
	ui.setupUi(this);

	QObject::connect(ui.sizeButton, &QPushButton::clicked, this, &Process::onSizeButtonClicked);
	QObject::connect(ui.readButton, &QPushButton::clicked, this, &Process::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &Process::onWriteButtonClicked);

	setWindowTitle("Memory Usage Monitor");
	ui.memoryUsageBar->setRange(0, 100);  // 范围设置为 0-100%

	// 创建共享内存默认大小4096
	if (!m_sharedMemory.Create(4096)) {
		qDebug() << "Failed to create shared memory.";
	}

	// 初始化定时器
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &Process::updateMemoryUsage);
	m_timer->start(1000); // 每秒更新内存使用情况
}

void Process::onSizeButtonClicked() {
	QString l_szSize = ui.sizeInput->text();
	int l_lSize = l_szSize.toInt();
	m_sharedMemory.Resize(l_lSize);
}

void Process::onReadButtonClicked() {
	QString data = m_sharedMemory.Read(); // 从共享内存读取数据
	if (!data.isEmpty()) {
		qDebug() << "A:Data read from shared memory:" << data;
		ui.valueOutput->setText(data);

		// 假设你的共享内存类有方法获取地址、大小和状态
		void* address = m_sharedMemory.getAddress(); // 获取共享内存地址
		size_t size = m_sharedMemory.getSize();      // 获取共享内存大小
		QString status = m_sharedMemory.getStatus();  // 获取共享内存状态

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
	if (m_sharedMemory.Write(dataToWrite)) {
		qDebug() << "A:Data written to shared memory:" << dataToWrite;
	}
	else {
		qDebug() << "A:Failed to write data to shared memory.";
	}

}

void Process::updateMemoryUsage() {
	double memoryUsagePercent = getMemoryUsage();
	ui.memoryUsageBar->setValue(static_cast<int>(memoryUsagePercent));
}

double Process::getMemoryUsage() {
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

	// 获取内存状态
	if (GlobalMemoryStatusEx(&memoryStatus)) {
		double totalMemory = memoryStatus.ullTotalPhys; // 物理内存总量
		double freeMemory = memoryStatus.ullAvailPhys;  // 可用物理内存

		// 计算已用内存的百分比
		double usedMemory = totalMemory - freeMemory;
		return (usedMemory / totalMemory) * 100.0;
	}
	return 0.0; // 获取内存状态失败
}