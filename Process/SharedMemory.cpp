#include "SharedMemory.h"

SharedMemory::SharedMemory(const QString& key) : sharedMemory(key) {}

bool SharedMemory::Create(int size) {
	if (sharedMemory.create(size)) {
		memoryAddress = sharedMemory.data(); // 获取共享内存的地址
		memorySize = sharedMemory.size();    // 获取共享内存的大小
		isInitialized = true;                // 设置为已初始化
		qDebug() << "Shared memory created successfully.";
		qDebug() << "Memory address:" << reinterpret_cast<quintptr>(memoryAddress);
		qDebug() << "Memory size:" << memorySize;
		return true;                        // 创建成功
	}
	else {
		qDebug() << "Unable to create shared memory:" << sharedMemory.errorString();
		return false;                       // 创建失败
	}
}

void SharedMemory::Detach() {
	sharedMemory.detach();
}

bool SharedMemory::Attach() {
	if (sharedMemory.attach()) {
		return true;
	}
	return false;
}

QString SharedMemory::Read() {
	if (!sharedMemory.isAttached()) {
		qDebug() << "Shared memory is not attached!";
		return QString();
	}

	const char* from = static_cast<const char*>(sharedMemory.constData());
	if (!from) {
		qDebug() << "Unable to get shared memory data!";
		return QString();
	}

	return QString::fromUtf8(from);
}


bool SharedMemory::Write(const QString& data) {
	if (!sharedMemory.isAttached()) {
		qDebug() << "Shared memory is not attached!";
		return false;
	}

	char* to = static_cast<char*>(sharedMemory.data());
	if (!to) {
		qDebug() << "Unable to get shared memory data!";
		return false;
	}

	// 清空共享内存并写入数据
	memset(to, 0, sharedMemory.size());

	// 使用 QByteArray 写入数据
	QByteArray byteArray = data.toUtf8();
	if (byteArray.size() < sharedMemory.size()) {
		memcpy(to, byteArray.constData(), byteArray.size());
	}
	else {
		qDebug() << "Data is too large for shared memory!";
		return false;
	}

	return true;
}

void* SharedMemory::getAddress() {
	return isInitialized ? memoryAddress : nullptr; // 返回地址或空指针
}

size_t SharedMemory::getSize() {
	return isInitialized ? memorySize : 0; // 返回大小或0
}

QString SharedMemory::getStatus() {
	return isInitialized ? "Initialized" : "Not Initialized"; // 返回状态
}
