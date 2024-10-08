#include "SharedMemory.h"

SharedMemory::SharedMemory(const QString& key) : sharedMemory(key) {}

bool SharedMemory::Create(int size) {
	if (sharedMemory.create(size)) {
		return true;
	}
	else {
		qDebug() << "Unable to create shared memory:" << sharedMemory.errorString();
		return false;
	}
}

void SharedMemory::Detach() {
	sharedMemory.detach();
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
