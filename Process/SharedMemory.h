#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <QSharedMemory>
#include <QString>
#include <QDebug>

class SharedMemory {
public:
	SharedMemory(const QString& key);
	bool Create(int size);
	bool Resize(int newSize);
	bool Attach();
	void Detach();
	bool Write(const QString& data);
	QString Read();
	void* getAddress(); // ��ȡ�����ڴ��ַ
	size_t getSize(); // ��ȡ�����ڴ��С
	QString getStatus(); // ��ȡ�����ڴ�״̬
private:
	QSharedMemory m_sharedMemory;
	void* m_memoryAddress; // �����ڴ�ĵ�ַ
	size_t m_szMemorySize;   // �����ڴ�Ĵ�С
	bool m_bIsInitialized;   // ״̬��ʶ
};

#endif // SHAREDMEMORY_H
