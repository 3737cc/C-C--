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

void Process::onReadButtonClicked() {
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	QString data = m_sharedMemory.Read(); // 从共享内存读取数据
	QStringList l_dataList = data.split(",");

	QueryPerformanceCounter(&m_end);
	double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
	std::cout << "读取运行时间： " << l_dInterval << " ms" << std::endl;

	if (l_dataList.isEmpty()) {
		qDebug() << "A:No data found in shared memory.";
	}
	else {
		qDebug() << "A:Data read from shared memory, total count: " << l_dataList.size();
		ui.valueOutput->setText(l_dataList.join(",")); // 显示所有读取的数据
	}
}

void Process::onWriteButtonClicked() {
	const int l_iNumberOfEntries = 1;
	const int l_iTestRuns = 10;  // 进行多次测试
	double l_dTotalTime = 0.0;   // 记录总时间
	QStringList l_dataToWrite;

	// 生成随机数据（每次生成相同的大小）
	for (int i = 0; i < l_iNumberOfEntries; ++i) {
		QString randomData = generateRandomString(1);  // 每条随机数据
		l_dataToWrite.append(randomData);
	}

	QString l_dataString = l_dataToWrite.join(",");  // 将所有数据合并成一个大字符串

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);  // 获取频率

	for (int run = 0; run < l_iTestRuns; ++run) {

		// 开始计时
		QueryPerformanceCounter(&start);

		// 执行写入操作
		m_sharedMemory.Write(l_dataString);

		// 结束计时
		QueryPerformanceCounter(&end);

		// 计算运行时间 (毫秒)
		double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
		std::cout << "运行时间 (" << run + 1 << "): " << interval << " ms" << std::endl;

		l_dTotalTime += interval;  // 累加时间
	}

	// 计算平均时间
	double l_dAverageTime = l_dTotalTime / l_iTestRuns;
	std::cout << "平均写入时间: " << l_dAverageTime << " ms" << std::endl;
}


void Process::updateMemoryUsage() {
	double l_dMemoryUsagePercent = getMemoryUsage();
	ui.memoryUsageBar->setValue(static_cast<int>(l_dMemoryUsagePercent));
}

double Process::getMemoryUsage() {
	MEMORYSTATUSEX l_memoryStatus;
	l_memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);

	// 获取内存状态
	if (GlobalMemoryStatusEx(&l_memoryStatus)) {
		double l_dTotalMemory = l_memoryStatus.ullTotalPhys; // 物理内存总量
		double l_dFreeMemory = l_memoryStatus.ullAvailPhys;  // 可用物理内存

		// 计算已用内存的百分比
		double l_dUsedMemory = l_dTotalMemory - l_dFreeMemory;
		return (l_dUsedMemory / l_dTotalMemory) * 100.0;
	}
	return 0.0; // 获取内存状态失败
}

QString Process::generateRandomString(int l_iLength) {
	const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	QString randomString;
	for (int i = 0; i < l_iLength; ++i) {
		int index = rand() % possibleCharacters.length();
		randomString.append(possibleCharacters.at(index));
	}
	return randomString;
}
