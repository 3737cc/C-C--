#include "Process.h"

Process::Process(QWidget* parent)
	: QMainWindow(parent)
	, m_sharedMemory("MySharedMemoryKey") // ��ʼ�������ڴ棬ʹ����ͬ�ļ�
{
	ui.setupUi(this);

	QObject::connect(ui.sizeButton, &QPushButton::clicked, this, &Process::onSizeButtonClicked);
	QObject::connect(ui.readButton, &QPushButton::clicked, this, &Process::onReadButtonClicked);
	QObject::connect(ui.writeButton, &QPushButton::clicked, this, &Process::onWriteButtonClicked);

	setWindowTitle("Memory Usage Monitor");
	ui.memoryUsageBar->setRange(0, 100);  // ��Χ����Ϊ 0-100%

	// ���������ڴ�Ĭ�ϴ�С4096
	if (!m_sharedMemory.Create(4096)) {
		qDebug() << "Failed to create shared memory.";
	}

	// ��ʼ����ʱ��
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &Process::updateMemoryUsage);
	m_timer->start(1000); // ÿ������ڴ�ʹ�����
}

void Process::onSizeButtonClicked() {
	QString l_szSize = ui.sizeInput->text();
	int l_lSize = l_szSize.toInt();
	m_sharedMemory.Resize(l_lSize);
}

void Process::onReadButtonClicked() {
	QString data = m_sharedMemory.Read(); // �ӹ����ڴ��ȡ����
	if (!data.isEmpty()) {
		qDebug() << "A:Data read from shared memory:" << data;
		ui.valueOutput->setText(data);

		// ������Ĺ����ڴ����з�����ȡ��ַ����С��״̬
		void* address = m_sharedMemory.getAddress(); // ��ȡ�����ڴ��ַ
		size_t size = m_sharedMemory.getSize();      // ��ȡ�����ڴ��С
		QString status = m_sharedMemory.getStatus();  // ��ȡ�����ڴ�״̬

		// ��memoryBlockTable�������
		int rowCount = ui.memoryBlockTable->rowCount();
		ui.memoryBlockTable->insertRow(rowCount); // ������������

		// ��������������
		ui.memoryBlockTable->setItem(rowCount, 0, new QTableWidgetItem(QString::number(reinterpret_cast<quintptr>(address)))); // ��ַ
		ui.memoryBlockTable->setItem(rowCount, 1, new QTableWidgetItem(QString::number(size))); // ��С
		ui.memoryBlockTable->setItem(rowCount, 2, new QTableWidgetItem(status)); // ״̬
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

	// ��ȡ�ڴ�״̬
	if (GlobalMemoryStatusEx(&memoryStatus)) {
		double totalMemory = memoryStatus.ullTotalPhys; // �����ڴ�����
		double freeMemory = memoryStatus.ullAvailPhys;  // ���������ڴ�

		// ���������ڴ�İٷֱ�
		double usedMemory = totalMemory - freeMemory;
		return (usedMemory / totalMemory) * 100.0;
	}
	return 0.0; // ��ȡ�ڴ�״̬ʧ��
}