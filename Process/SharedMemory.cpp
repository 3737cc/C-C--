#include "SharedMemory.h"

SharedMemory::SharedMemory(const QString& key) : sharedMemory(key) {}

bool SharedMemory::Create(int size) {
	if (sharedMemory.create(size)) {
		memoryAddress = sharedMemory.data(); // ��ȡ�����ڴ�ĵ�ַ
		memorySize = sharedMemory.size();    // ��ȡ�����ڴ�Ĵ�С
		isInitialized = true;                // ����Ϊ�ѳ�ʼ��
		qDebug() << "Shared memory created successfully.";
		qDebug() << "Memory address:" << reinterpret_cast<quintptr>(memoryAddress);
		qDebug() << "Memory size:" << memorySize;
		return true;                        // �����ɹ�
	}
	else {
		qDebug() << "Unable to create shared memory:" << sharedMemory.errorString();
		return false;                       // ����ʧ��
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

	// ��չ����ڴ沢д������
	memset(to, 0, sharedMemory.size());

	// ʹ�� QByteArray д������
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
	return isInitialized ? memoryAddress : nullptr; // ���ص�ַ���ָ��
}

size_t SharedMemory::getSize() {
	return isInitialized ? memorySize : 0; // ���ش�С��0
}

QString SharedMemory::getStatus() {
	return isInitialized ? "Initialized" : "Not Initialized"; // ����״̬
}
