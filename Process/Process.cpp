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
		qDebug() << "A:Data read from shared memory:" << data;
		ui.valueOutput->setText(data);

		// ������Ĺ����ڴ����з�����ȡ��ַ����С��״̬
		void* address = sharedMemory.getAddress(); // ��ȡ�����ڴ��ַ
		size_t size = sharedMemory.getSize();      // ��ȡ�����ڴ��С
		QString status = sharedMemory.getStatus();  // ��ȡ�����ڴ�״̬

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
	if (sharedMemory.Write(dataToWrite)) {
		qDebug() << "A:Data written to shared memory:" << dataToWrite;
	}
	else {
		qDebug() << "A:Failed to write data to shared memory.";
	}

}
