#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <QSharedMemory>
#include <QString>
#include <QDebug>

class SharedMemory {
public:
	SharedMemory(const QString& key);
	bool Create(int size);
	bool Attach();
	void Detach();
	bool Write(const QString& data);
	QString Read();
	void* getAddress(); // ��ȡ�����ڴ��ַ
	size_t getSize(); // ��ȡ�����ڴ��С
	QString getStatus(); // ��ȡ�����ڴ�״̬
private:
	QSharedMemory sharedMemory;
	void* memoryAddress; // �����ڴ�ĵ�ַ
	size_t memorySize;   // �����ڴ�Ĵ�С
	bool isInitialized;   // ״̬��ʶ
};

#endif // SHAREDMEMORY_H
