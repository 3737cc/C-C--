#include "SharedMemory.h"

SharedMemory::SharedMemory(const QString& key) : m_sharedMemory(key) {}

bool SharedMemory::Create(int size) {
	if (m_sharedMemory.create(size)) {
		m_memoryAddress = m_sharedMemory.data(); // ��ȡ�����ڴ�ĵ�ַ
		m_szMemorySize = m_sharedMemory.size();    // ��ȡ�����ڴ�Ĵ�С
		m_bIsInitialized = true;                // ����Ϊ�ѳ�ʼ��
		qDebug() << "Shared memory created successfully.";
		qDebug() << "Memory address:" << reinterpret_cast<quintptr>(m_memoryAddress);
		qDebug() << "Memory size:" << m_szMemorySize;
		return true;                        // �����ɹ�
	}
	else {
		qDebug() << "Unable to create shared memory:" << m_sharedMemory.errorString();
		return false;                       // ����ʧ��
	}
}

bool SharedMemory::Resize(int newSize) {
	// ���������еĹ����ڴ�
	Detach();
	m_bIsInitialized = false; // ���³�ʼ��״̬

	// �����µĹ����ڴ�
	if (m_sharedMemory.create(newSize)) {
		m_memoryAddress = m_sharedMemory.data(); // ��ȡ�¹����ڴ�ĵ�ַ
		m_szMemorySize = m_sharedMemory.size();    // ��ȡ�¹����ڴ�Ĵ�С
		m_bIsInitialized = true;                // ����Ϊ�ѳ�ʼ��
		qDebug() << "Shared memory resized successfully.";
		qDebug() << "New memory address:" << reinterpret_cast<quintptr>(m_memoryAddress);
		qDebug() << "New memory size:" << m_szMemorySize;
		return true;                        // �����ɹ�
	}
	else {
		qDebug() << "Unable to resize shared memory:" << m_sharedMemory.errorString();
		return false;                       // ����ʧ��
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

	// ��չ����ڴ沢д������
	memset(to, 0, m_sharedMemory.size());

	// ʹ�� QByteArray д������
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
	return m_bIsInitialized ? m_memoryAddress : nullptr; // ���ص�ַ���ָ��
}

size_t SharedMemory::getSize() {
	return m_bIsInitialized ? m_szMemorySize : 0; // ���ش�С��0
}

QString SharedMemory::getStatus() {
	return m_bIsInitialized ? "Initialized" : "Not Initialized"; // ����״̬
}
