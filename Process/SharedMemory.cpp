#include "SharedMemory.h"

SharedMemory::SharedMemory(const QString& key) : m_sharedMemory(key) {}

bool SharedMemory::Create(int size) {
	if (m_sharedMemory.create(size)) {
		m_memoryAddress = m_sharedMemory.data(); // 获取共享内存的地址
		m_szMemorySize = m_sharedMemory.size();    // 获取共享内存的大小
		m_bIsInitialized = true;                // 设置为已初始化
		qDebug() << "Shared memory created successfully.";
		qDebug() << "Memory address:" << reinterpret_cast<quintptr>(m_memoryAddress);
		qDebug() << "Memory size:" << m_szMemorySize;
		return true;                        // 创建成功
	}
	else {
		qDebug() << "Unable to create shared memory:" << m_sharedMemory.errorString();
		return false;                       // 创建失败
	}
}

bool SharedMemory::Resize(int newSize) {
	// 先销毁现有的共享内存
	Detach();
	m_bIsInitialized = false; // 更新初始化状态

	// 创建新的共享内存
	if (m_sharedMemory.create(newSize)) {
		m_memoryAddress = m_sharedMemory.data(); // 获取新共享内存的地址
		m_szMemorySize = m_sharedMemory.size();    // 获取新共享内存的大小
		m_bIsInitialized = true;                // 设置为已初始化
		qDebug() << "Shared memory resized successfully.";
		qDebug() << "New memory address:" << reinterpret_cast<quintptr>(m_memoryAddress);
		qDebug() << "New memory size:" << m_szMemorySize;
		return true;                        // 创建成功
	}
	else {
		qDebug() << "Unable to resize shared memory:" << m_sharedMemory.errorString();
		return false;                       // 创建失败
	}
}

void SharedMemory::Detach() {
	m_sharedMemory.detach();
}

bool SharedMemory::Attach() {
	if (m_sharedMemory.attach()) {
		return true;
	}
	return false;
}

QString SharedMemory::Read() {
	if (!m_sharedMemory.isAttached()) {
		qDebug() << "Shared memory is not attached!";
		return QString();
	}

	const char* from = static_cast<const char*>(m_sharedMemory.constData());
	if (!from) {
		qDebug() << "Unable to get shared memory data!";
		return QString();
	}

	return QString::fromUtf8(from);
}


bool SharedMemory::Write(const QString& data) {
	if (!m_sharedMemory.isAttached()) {
		qDebug() << "Shared memory is not attached!";
		return false;
	}

	char* to = static_cast<char*>(m_sharedMemory.data());
	if (!to) {
		qDebug() << "Unable to get shared memory data!";
		return false;
	}

	// 清空共享内存并写入数据
	memset(to, 0, m_sharedMemory.size());

	// 使用 QByteArray 写入数据
	QByteArray byteArray = data.toUtf8();
	if (byteArray.size() < m_sharedMemory.size()) {
		memcpy(to, byteArray.constData(), byteArray.size());
	}
	else {
		qDebug() << "Data is too large for shared memory!";
		return false;
	}

	return true;
}

void* SharedMemory::getAddress() {
	return m_bIsInitialized ? m_memoryAddress : nullptr; // 返回地址或空指针
}

size_t SharedMemory::getSize() {
	return m_bIsInitialized ? m_szMemorySize : 0; // 返回大小或0
}

QString SharedMemory::getStatus() {
	return m_bIsInitialized ? "Initialized" : "Not Initialized"; // 返回状态
}
