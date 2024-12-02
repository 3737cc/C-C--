#ifndef NIDAQ_COMMUNICATION_H
#define NIDAQ_COMMUNICATION_H

#include <vector>
#include <string>
#include "NIDAQmx.h"
#include <Qstring>
#include <QThread>
#include <atomic>
#include <QtXlsx>
#include <QSettings>
#include <fstream>
#include <queue>
//#include <condition_variable>

class NIDAQCommunication : public QObject {
    Q_OBJECT

public:
    NIDAQCommunication();
    ~NIDAQCommunication();

    bool Initialize(const std::string& iName);
    bool oInitialize(const std::string& Name);
    bool SaveParInfo(const QString& filename); // 保存配置
    bool LoadParInfo(const QString& filename); // 读取配置
    void InitParameters();  // 初始化参数
    bool InputConfigureChannel(); // 输入通道配置
    bool OutputConfigureChannel();
    // 输出通道配置
    bool StartAcquisition();    // 开始采集
    bool StartOutputAcquisition();  // 开始输出
    void StopAcquisition();
    void StopProcess();
    float64 OutputData(float64 data);
    QVector<float64> LoadDataFromExcel(const QString& filePath); // 保存数据到容器中
    void ProcessExcelAndOutput(const QString& filePath);
    bool PulseCom(); // 使用脉冲序列
    float64 GetData();
    std::string GetDeviceStatus() const;
    bool ResetDevice();
    void DeleteXlsx(); // 删除表格

    // 设置
    void SetTime(float64 time);
    void SetType();
    void SetInputChannel(std::string channel);
    void SetOutputChannel(std::string channel);
    void SetMinValue(float64 minvalue);
    void SetMaxValue(float64 maxvalue);
    void SetMinVolt(float64 minvolt);
    void SetMaxVolt(float64 maxvolt);
    void SetLoops(int loops);

    // 获取
    float64 GetTime() const;
    float64 GetMinValue() const;
    float64 GetMaxValue() const;
    float64 GetMinVolt() const;
    float64 GetMaxVolt() const;
    std::string GetInputChannel() const;
    int GetInputChannelIndex() const;
    std::string GetOutputChannel() const;
    int GetOutputChannelIndex() const;
    bool IsAcquiring() const;

public:
    struct NIDAQParameters {
        TaskHandle m_iTaskHandle ;  // 管理DAQmx模拟输入任务
        TaskHandle m_oTaskHandle ;  // 管理DAQmx模拟输出任务
        int m_iBufferSize;  // 每次读取的样本数量
        int32 m_iRead; // 记录成功读取的采样数
        float64 m_fData[1000]; // 存储采集到的数据
        float64 m_iTime; // 帧率设置
        float64 m_iType; // 系数设置
        std::string m_inputChannel; // 设置输入通道
        std::string m_outputChannel; // 设置输出通道
        float64 m_fMinValue;
        float64 m_fMaxValue;
        float64 m_fMinVolt;
        float64 m_fMaxVolt;
    }m_NIDAQParameters;

    float64 buffer[1];

signals:
    void DataAcquired(float64* data, int count);

private:
    void RunAcquisition(); 
    //void RunOutput();

private:
    std::string m_deviceName;
    bool m_isInitialized;
    bool m_isAcquiring;
    std::vector<double> m_dataBuffer;
    QThread* m_thread;           // 线程对象
    QThread* outputThread;
    std::atomic<bool> m_running; // 控制线程的运行标志
    std::atomic<bool> m_stop;    // 停止采集标志
    bool m_isTaskStarted = false;  // 任务启动标志位
    QXlsx::Document m_xlsx; // 创建XLSX
    std::atomic<bool> m_stopFlag; 
    QVector<float64> dataContainer; // 存储XLSX数据
    int m_iLoops;   // 循环次数

    bool m_stopInput = false;   // 停止输入标志
    bool m_stopOutput = false;  // 停止输出标志
    bool m_readWrite = false;

    std::mutex mtx;                     // 互斥锁保护共享资源
    std::condition_variable cv;         // 条件变量用于线程同步
    std::vector<float> m_mtxBuffer;          // 共享缓冲区
    bool dataAvailable = false;         // 标记数据是否可用
};

#endif // NIDAQ_COMMUNICATION_H
