#include "Process.h"

Process::Process(QWidget* parent)
	: QMainWindow(parent)
	, m_sharedMemory("MySharedMemoryKey") // 初始化共享内存，使用相同的键
{
	ui.setupUi(this);

	//QObject::connect(ui.sizeButton, &QPushButton::clicked, this, &Process::onSizeButtonClicked);
	QObject::connect(ui.readButton, &QPushButton::clicked, this, &Process::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &Process::onWriteButtonClicked);

	setWindowTitle("Memory Usage Monitor");
	ui.memoryUsageBar->setRange(0, 100);  // 范围设置为 0-100%

	// 创建共享内存默认大小4096
	if (!m_sharedMemory.Create(1024 * 1024 * 4)) {
		qDebug() << "Failed to create shared memory.";
	}

	// 初始化定时器
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &Process::updateMemoryUsage);
	m_timer->start(1000); // 每秒更新内存使用情况
}

//void Process::onSizeButtonClicked() {
//	QString l_szSize = ui.sizeInput->text();
//	int l_lSize = l_szSize.toInt();
//	m_sharedMemory.Resize(l_lSize);
//}

void Process::onReadButtonClicked() {
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	QString data = m_sharedMemory.Read(); // 从共享内存读取数据
	QStringList dataList = data.split(",");

	QueryPerformanceCounter(&end);
	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
	std::cout << "读取运行时间： " << interval << " ms" << std::endl;

	if (dataList.isEmpty()) {
		qDebug() << "A:No data found in shared memory.";
	}
	else {
		qDebug() << "A:Data read from shared memory, total count: " << dataList.size();
		ui.valueOutput->setText(dataList.join(",")); // 显示所有读取的数据
	}
}

//void Process::onReadButtonClicked() {
//	QueryPerformanceFrequency(&frequency);
//	QueryPerformanceCounter(&start);
//	QString data = m_sharedMemory.Read(); // 从共享内存读取数据
//	QueryPerformanceCounter(&end);
//	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
//	std::cout << "读取运行时间： " << interval << " ms" << std::endl;
//	if (!data.isEmpty()) {
//		qDebug() << "A:Data read from shared memory:" << data;
//		ui.valueOutput->setText(data);
//
//		void* address = m_sharedMemory.getAddress(); // 获取共享内存地址
//		size_t size = m_sharedMemory.getSize();      // 获取共享内存大小
//		QString status = m_sharedMemory.getStatus();  // 获取共享内存状态
//
//		// 在memoryBlockTable中添加行
//		int rowCount = ui.memoryBlockTable->rowCount();
//		ui.memoryBlockTable->insertRow(rowCount); // 在最后插入新行
//
//		// 将数据填入新行
//		ui.memoryBlockTable->setItem(rowCount, 0, new QTableWidgetItem(QString::number(reinterpret_cast<quintptr>(address)))); // 地址
//		ui.memoryBlockTable->setItem(rowCount, 1, new QTableWidgetItem(QString::number(size))); // 大小
//		ui.memoryBlockTable->setItem(rowCount, 2, new QTableWidgetItem(status)); // 状态
//	}
//	else {
//		qDebug() << "A:No data found in shared memory.";
//	}
//}

//void Process::onReadButtonClicked() {
//	QueryPerformanceFrequency(&frequency);
//	QueryPerformanceCounter(&start);
//
//	QString data = m_sharedMemory.Read(); // 从共享内存读取数据
//	QStringList dataList = data.split(","); // 按逗号分割成单个条目
//
//	QueryPerformanceCounter(&end);
//	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
//	std::cout << "总读取运行时间： " << interval << " ms" << std::endl;
//
//	// 每次读取1000条数据
//	int totalDataCount = dataList.size();
//	int batchSize = 1000;
//	int batches = totalDataCount / batchSize + (totalDataCount % batchSize == 0 ? 0 : 1);
//
//	for (int i = 0; i < batches; ++i) {
//		QueryPerformanceCounter(&start);
//
//		QStringList batchData = dataList.mid(i * batchSize, batchSize);
//		QString displayData = batchData.join(",");
//		qDebug() << "A:Reading batch " << i + 1 << " of " << batches << ", data size: " << batchData.size();
//
//		ui.valueOutput->setText(displayData); // 显示当前批次数据
//
//		QueryPerformanceCounter(&end);
//		interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
//		std::cout << "批次 " << i + 1 << " 读取时间：" << interval << " ms" << std::endl;
//	}
//
//	if (dataList.isEmpty()) {
//		qDebug() << "A:No data found in shared memory.";
//	}
//	else {
//		qDebug() << "A:Data read from shared memory, total count: " << totalDataCount;
//	}
//}


//void Process::onWriteButtonClicked() {
//	QueryPerformanceFrequency(&frequency);
//	QueryPerformanceCounter(&start);
//	QString dataToWrite = ui.valueInput->text();
//	int size = dataToWrite.size();
//	std::cout << "数据大小：" << size << std::endl;
//	if (m_sharedMemory.Write(dataToWrite)) {
//		qDebug() << "A:Data written to shared memory:" << dataToWrite;
//	}
//	else {
//		qDebug() << "A:Failed to write data to shared memory.";
//	}
//	QueryPerformanceCounter(&end);
//	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000 / frequency.QuadPart;
//	std::cout << "传输运行时间：" << interval << " ms" << std::endl;
//}
void Process::onWriteButtonClicked() {
	const int numberOfEntries = 1;
	const int testRuns = 10;  // 进行多次测试
	double totalTime = 0.0;   // 记录总时间
	QStringList dataToWrite;

	// 生成随机数据（每次生成相同的大小）
	for (int i = 0; i < numberOfEntries; ++i) {
		QString randomData = generateRandomString(1);  // 每条随机数据
		dataToWrite.append(randomData);
	}

	QString dataString = dataToWrite.join(",");  // 将所有数据合并成一个大字符串

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);  // 获取频率

	for (int run = 0; run < testRuns; ++run) {

		// 开始计时
		QueryPerformanceCounter(&start);

		// 执行写入操作
		m_sharedMemory.Write(dataString);

		// 结束计时
		QueryPerformanceCounter(&end);

		// 计算运行时间 (毫秒)
		double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		std::cout << "运行时间 (" << run + 1 << "): " << interval << " ms" << std::endl;

		totalTime += interval;  // 累加时间
	}

	// 计算平均时间
	double averageTime = totalTime / testRuns;
	std::cout << "平均写入时间: " << averageTime << " ms" << std::endl;
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

QString Process::generateRandomString(int length) {
	const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	QString randomString;
	for (int i = 0; i < length; ++i) {
		int index = rand() % possibleCharacters.length();
		randomString.append(possibleCharacters.at(index));
	}
	return randomString;
}
