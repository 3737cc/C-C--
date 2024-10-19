#include "ProcessB.h"

ProcessB::ProcessB(QWidget* parent)
	: QMainWindow(parent)
	, m_sharedMemory("SharedMemoryKey")
	, m_iMaxSharedMemory(1024 * 1024 * 4)
	, m_iTestRuns(1)
{
	ui.setupUi(this);

	QObject::connect(ui.readButton, &QPushButton::clicked, this, &ProcessB::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &ProcessB::onWriteButtonClicked);
	QObject::connect(ui.createButton, &QPushButton::clicked, this, &ProcessB::onCreateOrOpenClicked);
	setWindowTitle("Memory Usage MonitorB");

	ui.memoryBlockTable->setRowCount(0);
	ui.memoryBlockTable->setColumnCount(4);
}

void ProcessB::onCreateOrOpenClicked() {
	// 连接共享内存
	if (!m_sharedMemory.Attach()) {
		qDebug() << "Unable to connect to shared memory.";
	}
	//// 创建并初始化队列
	//if (!m_sharedMemory.Create(m_iMaxSharedMemory)) {
	//	qDebug() << "Failed to create shared memory.";
	//}
	//else {
	//	// 初始化队列，设置每个元素的最大大小为1KB
	//	if (!m_sharedMemory.InitializeQueue(1024)) {
	//		qDebug() << "Failed to initialize queue.";
	//	}
	//}
}

void ProcessB::onWriteButtonClicked() {
	if (m_sharedMemory.IsFull()) {
		QMessageBox::warning(this, "Warning", "Queue is full!");
		return;
	}

	double l_dTotalTime = 0.0;
	QString l_dataString = ui.sendTextEdit->toPlainText();

	LARGE_INTEGER frequency, start, end;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	// 将数据入队
	if (!m_sharedMemory.Enqueue(l_dataString)) {
		QMessageBox::warning(this, "Error", "Failed to write to queue!");
		return;
	}

	QueryPerformanceCounter(&end);
	double interval = static_cast<double>(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
	std::cout << "运行时间 " << interval << " ms" << std::endl;

	// 更新表格
	int l_iNewRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(l_iNewRow);
	size_t l_szDataSize = m_sharedMemory.GetQueueSize();
	QByteArray l_byteArray = l_dataString.toUtf8();
	int l_iByteSize = l_byteArray.size();

	m_shared = new QTableWidgetItem(QString::number(m_iMaxSharedMemory));
	m_numberOfEntries = new QTableWidgetItem(QString::number(l_szDataSize));
	m_byte = new QTableWidgetItem(QString::number(l_iByteSize));
	m_time = new QTableWidgetItem(QString::number(interval));

	ui.memoryBlockTable->setItem(l_iNewRow, 0, m_shared);
	ui.memoryBlockTable->setItem(l_iNewRow, 1, m_numberOfEntries);
	ui.memoryBlockTable->setItem(l_iNewRow, 2, m_byte);
	ui.memoryBlockTable->setItem(l_iNewRow, 3, m_time);
	ui.memoryBlockTable->scrollToBottom();
}

void ProcessB::onReadButtonClicked() {
	if (m_sharedMemory.IsEmpty()) {
		QMessageBox::warning(this, "Warning", "Queue is empty!");
		return;
	}

	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_start);

	// 从队列中读取数据
	QString l_data = m_sharedMemory.Dequeue();

	QueryPerformanceCounter(&m_end);
	double l_dInterval = static_cast<double>(m_end.QuadPart - m_start.QuadPart) * 1000 / m_frequency.QuadPart;
	std::cout << "读取运行时间： " << l_dInterval << " ms" << std::endl;

	if (l_data.isEmpty()) {
		qDebug() << "A:No data found in shared memory.";
	}
	else {
		qDebug() << "A:Data read from shared memory: " << l_data;
		ui.readTextEdit->setPlainText(l_data);
	}

	// 更新表格
	int l_iNewRow = ui.memoryBlockTable->rowCount();
	ui.memoryBlockTable->insertRow(l_iNewRow);
	size_t l_szDataSize = m_sharedMemory.GetQueueSize();
	QByteArray l_byteArray = l_data.toUtf8();
	int l_iByteSize = l_byteArray.size();

	m_shared = new QTableWidgetItem(QString::number(m_iMaxSharedMemory));
	m_numberOfEntries = new QTableWidgetItem(QString::number(l_szDataSize));
	m_byte = new QTableWidgetItem(QString::number(l_iByteSize));
	m_time = new QTableWidgetItem(QString::number(l_dInterval));

	ui.memoryBlockTable->setItem(l_iNewRow, 0, m_shared);
	ui.memoryBlockTable->setItem(l_iNewRow, 1, m_numberOfEntries);
	ui.memoryBlockTable->setItem(l_iNewRow, 2, m_byte);
	ui.memoryBlockTable->setItem(l_iNewRow, 3, m_time);
	ui.memoryBlockTable->scrollToBottom();
}
