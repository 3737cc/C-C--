#ifndef ARTCOMMUNICATION_H
#define ARTCOMMUNICATION_H

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "Art_DAQ.h"

class ArtCommunication {
public:
    // 构造函数
    explicit ArtCommunication();

    // 析构函数
    ~ArtCommunication();

    // 初始化DAQ设备
    bool Initialize();

    // 读取电压数据
    bool ReadVoltageData(std::vector<double>& voltageData);

    // 写入电压数据
    bool WriteVoltageData(const std::vector<double>& voltageData);

    // 关闭设备
    void CloseDevice();

private:
    // 初始化内部参数
    void InitParameters();

    // 常量定义
    static constexpr double PI = 3.14159265358979323846;
    static constexpr int MAX_DATA_POINTS = 1000;
    static constexpr double SAMPLE_RATE = 10000.0;
    static constexpr double MIN_VOLTAGE = -10.0;
    static constexpr double MAX_VOLTAGE = 10.0;

    // 设备参数结构体
    struct DeviceParameters {
        TaskHandle taskHandle;
        float64 data[MAX_DATA_POINTS];
        char errorBuffer[2048];
    };

    std::string m_deviceName;       // 设备名称
    bool m_isInitialized;           // 是否已初始化
    DeviceParameters m_deviceParams; // 设备参数
};

#endif // ARTCOMMUNICATION_H