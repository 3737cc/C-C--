#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <QSharedMemory>
#include <QString>
#include <QDebug>

#ifdef SHAREDMEMORY_EXPORTS
#define SHAREDMEMORYAPI __declspec(dllexport)
#else
#define SHAREDMEMORYAPI __declspec(dllimport)
#endif

// 队列头部结构，用于管理队列状态
struct QueueHeader {
	int head;           // 队列头部位置
	int tail;           // 队列尾部位置
	size_t capacity;    // 队列容量(元素个数)
	size_t elementSize; // 每个元素的大小
	size_t usedSize;    // 已使用的大小
};

class SHAREDMEMORYAPI SharedMemory {
public:
	SharedMemory(const QString& key);
	~SharedMemory();

	// 原有接口
	bool Create(int size);
	bool Resize(int newSize);
	bool Attach();
	void Detach();
	bool Write(const QString& data);
	QString Read();
	void* getAddress();
	size_t getSize();

	QByteArray readData();

	// 队列相关接口
	bool InitializeQueue(size_t elementSize);    // 初始化队列
	bool Enqueue(const QString& data);          // 入队
	QString Dequeue();                          // 出队
	bool IsEmpty() const;                       // 检查队列是否为空
	bool IsFull() const;                        // 检查队列是否已满
	size_t GetQueueSize() const;                // 获取当前队列中的元素数量
	size_t GetAvailableSpace() const;           // 获取可用空间

private:
	QSharedMemory m_sharedMemory;
	void* m_memoryAddress;                      // 共享内存的地址
	size_t m_szMemorySize;                      // 共享内存的大小
	bool m_bIsInitialized;                      // 状态标识

	// 队列相关的私有成员
	QueueHeader* m_header;                      // 指向队列头部信息的指针
	char* m_data;                              // 指向数据区域的指针

	// 私有辅助方法
	void InitializeMemoryLayout();              // 初始化内存布局
	//QueueHeader* GetHeader() const;             // 获取队列头部信息
	//char* GetDataRegion() const;                // 获取数据区域
	void UpdateMemoryPointers();                // 更新内存指针
};

#endif // SHAREDMEMORY_H