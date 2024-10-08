#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <QSharedMemory>
#include <QString>
#include <QDebug>

class SharedMemory {
public:
	SharedMemory(const QString& key);
	bool Create(int size);
	void Detach();
	bool Write(const QString& data);
	QString Read();

private:
	QSharedMemory sharedMemory;
};

#endif // SHAREDMEMORY_H
