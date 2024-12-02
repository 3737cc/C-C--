#include "NIDAQWorker.h"
#include <QDebug>

NIDAQWorker::NIDAQWorker(NIDAQCommunication* communication, QObject* parent)
    : QThread(parent), m_communication(communication), m_stopFlag(false) {
}

NIDAQWorker::~NIDAQWorker() {
    stop();
    wait();
}

void NIDAQWorker::stop() {
    m_stopFlag = true;
}

void NIDAQWorker::run() {
    if (!m_communication->StartAcquisition()) {
        emit dataAcquired(nullptr, 0); // 通知采集失败
        return;
    }

    float64 buffer[1000];
    while (!m_stopFlag) {
        int32_t samplesRead = 0;
        int32_t status = DAQmxReadAnalogF64(
            m_communication->GetTaskHandle(), // 任务句柄
            1000,                            // 每次读取1000个样本
            1.0,                             // 超时时间1秒
            DAQmx_Val_GroupByChannel,        // 数据组织方式
            buffer,                          // 缓冲区
            1000,                            // 缓冲区大小
            &samplesRead,                    // 实际读取的样本数量
            nullptr                          // 保留字段
        );

        if (status == 0 && samplesRead > 0) {
            emit dataAcquired(buffer, samplesRead); // 发出新数据信号
        }
        else {
            char errBuff[2048] = { 0 };
            DAQmxGetExtendedErrorInfo(errBuff, sizeof(errBuff));
            qWarning() << "Error reading data:" << errBuff;
            break;
        }

        QThread::msleep(10); // 控制采集频率，避免占用过多CPU资源
    }

    m_communication->StopAcquisition();
}
