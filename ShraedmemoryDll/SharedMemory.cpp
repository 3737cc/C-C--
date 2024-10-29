#include "SharedMemory.h"
#include <cstring>

SharedMemory::SharedMemory(const QString& key)
	: m_sharedMemory(key)
	, m_memoryAddress(nullptr)
	, m_szMemorySize(0)
	, m_bIsInitialized(false)
	, m_header(nullptr)
	, m_data(nullptr)
{}

SharedMemory::~SharedMemory() {
	Detach(); // 释放共享内存
}

void SharedMemory::setKey(const QString& key) {
	m_sharedMemory.setKey(key);
}

bool SharedMemory::Create(int size) {
	// 确保size足够大以容纳队列头部信息
	size_t minSize = sizeof(QueueHeader);
	if (size < minSize) {
		size = minSize;
	}

	if (m_sharedMemory.create(size)) {
		UpdateMemoryPointers();
		m_bIsInitialized = true;
		// 初始化队列头部信息
		if (m_header) {
			m_header->head = 0;
			m_header->tail = 0;
			m_header->capacity = (size - sizeof(QueueHeader)) / sizeof(char);
			m_header->elementSize = 0;  // 需要通过InitializeQueue设置
			m_header->usedSize = 0;
		}
		return true;
	}
	return false;
}

void SharedMemory::UpdateMemoryPointers() {
	m_memoryAddress = m_sharedMemory.data();
	m_szMemorySize = m_sharedMemory.size();
	InitializeMemoryLayout();
}

void SharedMemory::InitializeMemoryLayout() {
	if (m_memoryAddress) {
		m_header = static_cast<QueueHeader*>(m_memoryAddress);
		m_data = static_cast<char*>(m_memoryAddress) + sizeof(QueueHeader);
	}
}

bool SharedMemory::InitializeQueue(size_t elementSize) {
	if (!m_bIsInitialized || !m_header) {
		return false;
	}

	size_t availableSpace = m_szMemorySize - sizeof(QueueHeader);
	m_header->elementSize = elementSize;
	m_header->capacity = availableSpace / elementSize;
	m_header->head = m_header->tail = 0;
	m_header->usedSize = 0;

	return true;
}

bool SharedMemory::Enqueue(const QString& data) {
	if (!m_bIsInitialized || !m_header || IsFull()) {
		return false;
	}

	QByteArray byteArray = data.toUtf8();
	if (byteArray.size() > m_header->elementSize) {
		qDebug() << "Data is too large for queue element";
		return false;
	}

	// 计算写入位置
	char* writePos = m_data + (m_header->tail * m_header->elementSize);

	// 清空并写入数据
	memset(writePos, 0, m_header->elementSize);
	memcpy(writePos, byteArray.constData(), byteArray.size());

	// 更新尾指针和使用量
	m_header->tail = (m_header->tail + 1) % m_header->capacity;
	m_header->usedSize++;

	return true;
}

QString SharedMemory::Dequeue() {
	if (!m_bIsInitialized || !m_header || IsEmpty()) {
		return QString();
	}

	// 计算读取位置
	char* readPos = m_data + (m_header->head * m_header->elementSize);

	// 读取数据
	QString result = QString::fromUtf8(readPos);

	// 更新头指针和使用量
	m_header->head = (m_header->head + 1) % m_header->capacity;
	m_header->usedSize--;

	return result;
}

bool SharedMemory::IsEmpty() const {
	return m_header ? m_header->usedSize == 0 : true;
}

bool SharedMemory::IsFull() const {
	return m_header ? m_header->usedSize >= m_header->capacity : true;
}

size_t SharedMemory::GetQueueSize() const {
	return m_header ? m_header->usedSize : 0;
}

size_t SharedMemory::GetAvailableSpace() const {
	return m_header ? m_header->capacity - m_header->usedSize : 0;
}

// 原有方法的实现
bool SharedMemory::Resize(int newSize) {
	Detach();
	m_bIsInitialized = false;
	return Create(newSize);
}

void SharedMemory::Detach() {
	m_sharedMemory.detach();
	m_memoryAddress = nullptr;
	m_header = nullptr;
	m_data = nullptr;
	m_bIsInitialized = false;
}

bool SharedMemory::Attach() {
	if (m_sharedMemory.attach()) {
		UpdateMemoryPointers();
		m_bIsInitialized = true;
		return true;
	}
	return false;
}

// 为了保持向后兼容，保留原有的Read和Write方法
QString SharedMemory::Read() {
	return Dequeue();  // 现在Read等同于Dequeue
}

bool SharedMemory::Write(const QString& data) {
	return Enqueue(data);  // 现在Write等同于Enqueue
}

void* SharedMemory::getAddress() {
	return m_bIsInitialized ? m_memoryAddress : nullptr;
}

size_t SharedMemory::getSize() {
	return m_bIsInitialized ? m_szMemorySize : 0;
}
