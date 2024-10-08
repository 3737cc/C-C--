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
	void* getAddress(); // 获取共享内存地址
	size_t getSize(); // 获取共享内存大小
	QString getStatus(); // 获取共享内存状态
private:
	QSharedMemory m_sharedMemory;
	void* m_memoryAddress; // 共享内存的地址
	size_t m_szMemorySize;   // 共享内存的大小
	bool m_bIsInitialized;   // 状态标识
};

#endif // SHAREDMEMORY_H
