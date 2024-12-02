#ifndef NIDAQWORKER_H
#define NIDAQWORKER_H

#include <QThread>
#include "NIDAQCommunication.h"

class NIDAQWorker : public QThread {
    Q_OBJECT

public:
    explicit NIDAQWorker(NIDAQCommunication* communication, QObject* parent = nullptr);
    ~NIDAQWorker();

    void stop();

signals:
    void DataAcquired(float64* data, int count);

protected:
    void run() override;

private:
    NIDAQCommunication* m_communication;
    bool m_stopFlag;
};

#endif // NIDAQWORKER_H
