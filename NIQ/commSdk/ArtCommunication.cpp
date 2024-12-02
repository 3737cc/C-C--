#include "ArtCommunication.h"

ArtCommunication::ArtCommunication()
    : m_deviceName("Art"),
    m_isInitialized(false) {
    InitParameters();
}

ArtCommunication::~ArtCommunication() {
    if (m_isInitialized) {
        CloseDevice();
    }
}

void ArtCommunication::InitParameters() {
    m_deviceParams.taskHandle = 0;
    memset(m_deviceParams.data, 0, sizeof(m_deviceParams.data));
    memset(m_deviceParams.errorBuffer, 0, sizeof(m_deviceParams.errorBuffer));
}

bool ArtCommunication::Initialize() {
    try {
        // 创建任务
        if (ArtDAQ_CreateTask("", &m_deviceParams.taskHandle) != 0) {
            std::cerr << "Failed to create task" << std::endl;
            return false;
        }
        m_isInitialized = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

bool ArtCommunication::ReadVoltageData(std::vector<double>& voltageData) {
    if (!m_isInitialized) {
        std::cerr << "DAQ device not initialized." << std::endl;
        return false;
    }

    try {
        // 配置模拟输入通道
        if (ArtDAQ_CreateAIVoltageChan(m_deviceParams.taskHandle,
            "Dev4/ai0",
            "",
            ArtDAQ_Val_Cfg_Default,
            MIN_VOLTAGE,
            MAX_VOLTAGE,
            ArtDAQ_Val_Volts,
            NULL) != 0) {
            std::cerr << "Failed to create AI voltage channel" << std::endl;
            return false;
        }

        // 配置采样时钟
        if (ArtDAQ_CfgSampClkTiming(m_deviceParams.taskHandle,
            "",
            SAMPLE_RATE,
            ArtDAQ_Val_Rising,
            ArtDAQ_Val_ContSamps,
            MAX_DATA_POINTS) != 0) {
            std::cerr << "Failed to configure sampling clock" << std::endl;
            return false;
        }

        // 启动任务
        if (ArtDAQ_StartTask(m_deviceParams.taskHandle) != 0) {
            std::cerr << "Failed to start task" << std::endl;
            return false;
        }

        // 确保 voltageData 足够存储数据
        voltageData.resize(MAX_DATA_POINTS);

        int32 sampsPerChanRead = 0;  // 用于存储实际读取的样本数
        bool32 reserved = false;     // 保留参数，通常设置为 false

        // 读取数据到 voltageData
        int32 result = ArtDAQ_ReadAnalogF64(m_deviceParams.taskHandle,
            MAX_DATA_POINTS,       // 每通道读取的样本数
            10.0,                  // 超时时间（秒）
            ArtDAQ_Val_GroupByChannel,  // 数据填充模式
            voltageData.data(),    // 存储数据的数组
            MAX_DATA_POINTS,       // 数组的大小
            &sampsPerChanRead,     // 实际读取的样本数
            &reserved);            // 保留参数

        // 检查是否读取成功
        if (result != 0) {
            std::cerr << "Failed to read voltage data" << std::endl;
            return false;
        }

        std::cout << "Successfully read " << sampsPerChanRead << " samples." << std::endl;

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Read voltage data error: " << e.what() << std::endl;
        return false;
    }
}

bool ArtCommunication::WriteVoltageData(const std::vector<double>& voltageData) {
    if (!m_isInitialized) {
        std::cerr << "DAQ device not initialized." << std::endl;
        return false;
    }

    try {
        // 生成正弦波电压数据
        for (int i = 0; i < MAX_DATA_POINTS; i++) {
            m_deviceParams.data[i] = 9.95 * sin(static_cast<double>(i) * 2.0 * PI / MAX_DATA_POINTS);
        }

        // 配置模拟输出通道
        if (ArtDAQ_CreateAOVoltageChan(m_deviceParams.taskHandle,
            "Dev4/ao0",
            "",
            MIN_VOLTAGE,
            MAX_VOLTAGE,
            ArtDAQ_Val_Volts,
            "") != 0) {
            std::cerr << "Failed to create AO voltage channel" << std::endl;
            return false;
        }

        // 配置采样时钟
        if (ArtDAQ_CfgSampClkTiming(m_deviceParams.taskHandle,
            "",
            SAMPLE_RATE,
            ArtDAQ_Val_Rising,
            ArtDAQ_Val_ContSamps,
            MAX_DATA_POINTS) != 0) {
            std::cerr << "Failed to configure sampling clock" << std::endl;
            return false;
        }

        // 写入模拟电压数据
        if (ArtDAQ_WriteAnalogF64(m_deviceParams.taskHandle,
            MAX_DATA_POINTS,
            0,
            10.0,
            ArtDAQ_Val_GroupByChannel,
            m_deviceParams.data,
            NULL,
            NULL) != 0) {
            std::cerr << "Failed to write analog data" << std::endl;
            return false;
        }

        // 启动任务
        if (ArtDAQ_StartTask(m_deviceParams.taskHandle) != 0) {
            std::cerr << "Failed to start task" << std::endl;
            return false;
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Write voltage data error: " << e.what() << std::endl;
        return false;
    }
}

void ArtCommunication::CloseDevice() {
    if (m_deviceParams.taskHandle) {
        // 停止并清除任务
        ArtDAQ_StopTask(m_deviceParams.taskHandle);
        ArtDAQ_ClearTask(m_deviceParams.taskHandle);
        m_isInitialized = false;
    }
}