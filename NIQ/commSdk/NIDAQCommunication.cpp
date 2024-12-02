#include "NIDAQCommunication.h"
#include <iostream>
#include <stdexcept>
#include <QDebug>
#include <chrono>

// 构造函数
NIDAQCommunication::NIDAQCommunication()
    : m_thread(nullptr), m_running(false), m_stop(false), m_isInitialized(false), m_isAcquiring(false), m_stopFlag(false)
{
    InitParameters();
}

// 析构函数
NIDAQCommunication::~NIDAQCommunication() {
    StopAcquisition(); // 确保停止采集
}

// 初始化硬件设备
bool NIDAQCommunication::Initialize(const std::string& Name) {
    // 如果已经初始化，先清除之前的任务
    if (m_isInitialized) {
        if (m_NIDAQParameters.m_iTaskHandle != 0) {
            DAQmxStopTask(m_NIDAQParameters.m_iTaskHandle);
            DAQmxClearTask(m_NIDAQParameters.m_iTaskHandle);
        }
        m_NIDAQParameters.m_iTaskHandle = 0;
        m_isInitialized = false;
    }

    // 重新创建任务
    int32_t i_iIstatus = DAQmxCreateTask(Name.c_str(), &m_NIDAQParameters.m_iTaskHandle);
    if (i_iIstatus != 0) {
        char errBuff[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuff, sizeof(errBuff));
        std::cerr << "Error initializing NIDAQ device: " << errBuff << std::endl;
        m_isInitialized = false;
        return false;
    }

    m_isInitialized = true;
    return true;
}
// 初始化硬件设备
bool NIDAQCommunication::oInitialize(const std::string& Name) {
    // 如果已经初始化，先清除之前的任务
    if (m_isInitialized) {
        if (m_NIDAQParameters.m_oTaskHandle != 0) {
            DAQmxStopTask(m_NIDAQParameters.m_oTaskHandle);
            DAQmxClearTask(m_NIDAQParameters.m_oTaskHandle);
        }
        m_NIDAQParameters.m_oTaskHandle = 0;
        m_isInitialized = false;
    }

    // 重新创建任务
    int32_t i_iIstatus = DAQmxCreateTask(Name.c_str(), &m_NIDAQParameters.m_oTaskHandle);
    if (i_iIstatus != 0) {
        char errBuff[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuff, sizeof(errBuff));
        std::cerr << "Error initializing NIDAQ device: " << errBuff << std::endl;
        m_isInitialized = false;
        return false;
    }

    m_isInitialized = true;
    return true;
}

bool NIDAQCommunication::SaveParInfo(const QString& filename)
{
    std::ofstream file(filename.toStdString(), std::ios::binary);
    if (!file.is_open()) {
        //throw std::runtime_error("Failed to open file for writing");
        return false;
    }

    // 写入结构体数据
    file.write(reinterpret_cast<const char*>(&m_NIDAQParameters), sizeof(m_NIDAQParameters));

    file.close();
    return true;
}

bool NIDAQCommunication::LoadParInfo(const QString& filename)
{
    std::ifstream file(filename.toStdString(), std::ios::binary);
    if (!file.is_open()) {
        //throw std::runtime_error("Failed to open file for reading");
        return false;
    }

    // 读取结构体数据
    file.read(reinterpret_cast<char*>(&m_NIDAQParameters), sizeof(m_NIDAQParameters));

    file.close();
    return true;
}

// 初始化参数
void NIDAQCommunication::InitParameters() {
    m_iLoops = -1;
    m_NIDAQParameters.m_iTaskHandle = 0;
    m_NIDAQParameters.m_oTaskHandle = 0;
    m_NIDAQParameters.m_iBufferSize = 1;   // 每次读取的样本数量
    m_NIDAQParameters.m_iTime = 500;  // 默认读取帧率
    m_NIDAQParameters.m_inputChannel = "Dev2/ai1";
    m_NIDAQParameters.m_outputChannel = "Dev2/ao1";
    m_NIDAQParameters.m_fMinValue = -5.0;
    m_NIDAQParameters.m_fMaxValue = 5.0;
    m_NIDAQParameters.m_fMinVolt = -5.0;
    m_NIDAQParameters.m_fMaxVolt = 5;
    m_NIDAQParameters.m_iType = (m_NIDAQParameters.m_fMaxVolt -m_NIDAQParameters.m_fMinVolt)
        /(m_NIDAQParameters.m_fMaxValue-m_NIDAQParameters.m_fMinValue); // 默认系数为1.0
}

// 配置通道
bool NIDAQCommunication::InputConfigureChannel() {
    if (!m_isInitialized) {
        std::cerr << "Device not initialized!" << std::endl;
        return false;
    }
    // 配置模拟输入电压通道
    DAQmxCreateAIVoltageChan(
        m_NIDAQParameters.m_iTaskHandle,
        m_NIDAQParameters.m_inputChannel.c_str() ,// 通道名
        "",                              // 通道别名
        DAQmx_Val_RSE,           // 输入配置(单端输入)
        m_NIDAQParameters.m_fMinVolt,                           // 最小值
        m_NIDAQParameters.m_fMaxVolt,                            // 最大值
        DAQmx_Val_Volts,                 // 单位
        0);

    //// 配置时钟为连续采样模式
    //DAQmxCfgSampClkTiming(
    //    m_NIDAQParameters.m_iTaskHandle,
    //    "",                              // 使用默认时钟源
    //    m_NIDAQParameters.m_iTime,       // 采样率
    //    DAQmx_Val_Rising,                // 上升沿采样
    //    DAQmx_Val_ContSamps,             // 连续采样模式
    //    1000);                          // 缓存大小

    return true;
}

bool NIDAQCommunication::OutputConfigureChannel()
{
    if (!m_isInitialized) {
        std::cerr << "Device not initialized!" << std::endl;
        return false;
    }
    DAQmxCreateAOVoltageChan(
        m_NIDAQParameters.m_oTaskHandle,
        m_NIDAQParameters.m_outputChannel.c_str(),
        "",
        m_NIDAQParameters.m_fMinValue,
        m_NIDAQParameters.m_fMaxValue,
        DAQmx_Val_Volts,
        0);

    //DAQmxCfgSampClkTiming(
    //    m_NIDAQParameters.m_oTaskHandle,
    //    "",
    //    m_NIDAQParameters.m_iTime,
    //    DAQmx_Val_Rising,
    //    DAQmx_Val_OnDemand,
    //    1000);

    return true;
}

// 开始数据输入
bool NIDAQCommunication::StartOutputAcquisition() {
    if (!m_isInitialized) {
        std::cerr << "Error: Device not initialized!" << std::endl;
        return false;
    }

    //if (m_isAcquiring) {
    //    std::cerr << "Warning: Data acquisition is already in progress." << std::endl;
    //    return true;
    //}

    // 启动任务
    int32_t i_iStatus = DAQmxStartTask(m_NIDAQParameters.m_oTaskHandle);
    if (i_iStatus != 0) {
        char errBuff[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuff, sizeof(errBuff));
        std::cerr << "Error starting task: " << errBuff << std::endl;
        return false;
    }

    // 启动线程
    m_stopFlag = false;

    // 启动输出线程
    m_stopOutput = false;
    //LoadDataFromExcel("NI-Input.xlsx");
    outputThread = QThread::create([this]() { ProcessExcelAndOutput("NI-Input.xlsx"); });
    connect(outputThread, &QThread::finished, outputThread, &QObject::deleteLater);
    outputThread->start();

    m_isAcquiring = true;
    return true;
}// 开始数据输入

bool NIDAQCommunication::StartAcquisition() {
    if (!m_isInitialized) {
        std::cerr << "Error: Device not initialized!" << std::endl;
        return false;
    }

    //if (m_isAcquiring) {
    //    std::cerr << "Warning: Data acquisition is already in progress." << std::endl;
    //    return true;
    //}

    // 启动任务
    int32_t i_iStatus = DAQmxStartTask(m_NIDAQParameters.m_iTaskHandle);
    if (i_iStatus != 0) {
        char errBuff[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(errBuff, sizeof(errBuff));
        std::cerr << "Error starting task: " << errBuff << std::endl;
        return false;
    }

    // 启动线程
    m_running = true;

    // 启动采集线程
    m_stopInput = false;
    m_thread = QThread::create([this]() { RunAcquisition(); });
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    m_thread->start();

    m_isAcquiring = true;
    return true;
}

void NIDAQCommunication::RunAcquisition() {
    int row = 1;
    //std::vector<double> lastBuffer(m_NIDAQParameters.m_iBufferSize, 0.0);  // 初始化一个存储上次读取数据的缓冲区
    while (m_running) {
        // 确保缓冲区不会溢出
        int32_t readSize = m_NIDAQParameters.m_iBufferSize;

        auto start = std::chrono::high_resolution_clock::now();
        if (m_readWrite) {
            int32_t status = DAQmxReadAnalogF64(
                m_NIDAQParameters.m_iTaskHandle,   // 任务句柄
                readSize,                          // 每次读取的样本数量
                10.0,                               // 超时时间
                DAQmx_Val_GroupByChannel,          // 数据组织方式
                buffer,                            // 数据缓冲区
                10,                                 // 缓冲区大小
                &m_NIDAQParameters.m_iRead,        // 实际读取的样本数量
                nullptr                            // 保留字段
            );

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;

            // 严格的错误处理
            if (status != 0) {
                char errBuff[2048] = { 0 };
                DAQmxGetExtendedErrorInfo(errBuff, sizeof(errBuff));
                qWarning() << "Error reading data:" << errBuff;

                // 如果遇到严重错误，停止采集
                m_stopInput = true;
                break;
            }

            //bool isDataChanged = false;
            //for (int i = 0; i < readSize; ++i) {
            //    if (buffer[i] != lastBuffer[i]) {
            //        isDataChanged = true;
            //        break;
            //    }
            //}

            //// 如果数据发生变化，才处理数据
            //if (isDataChanged && m_NIDAQParameters.m_iRead > 0 && !m_stopFlag) {
            //    m_xlsx.write(row, 1, buffer[0]);
            //    m_xlsx.write(row, 2, (buffer[0] * m_NIDAQParameters.m_iType));
            //    m_xlsx.write(row, 3, QString::number(duration.count()) + "ms");
            //    emit DataAcquired(buffer, m_NIDAQParameters.m_iRead);
            //    // 更新上次的缓冲区数据
            //    lastBuffer.assign(buffer, buffer + readSize);
            //    ++row;
            //}
            // 数据，发射信号
            if (m_NIDAQParameters.m_iRead > 0 && !m_stopFlag) {
                m_xlsx.write(row, 1, buffer[0]);
                m_xlsx.write(row, 2, (buffer[0] * m_NIDAQParameters.m_iType));
                m_xlsx.write(row, 3, QString::number(duration.count()) + "ms");
                emit DataAcquired(buffer, m_NIDAQParameters.m_iRead);
            }
            row++;
            m_readWrite = false;
        }
        //QThread::msleep(qMax(1, static_cast<int>(1.0 / m_NIDAQParameters.m_iTime * 1000)));

        // 判断是否需要停止
        if (m_stopInput) {
            break; // 停止采集
        }
    }
    m_xlsx.saveAs("NI-Output.xlsx");
    m_running = false;
}

//void NIDAQCommunication::RunOutput() {
//    ProcessExcelAndOutput("NI-Input.xlsx");
//}

float64 NIDAQCommunication::OutputData(float64 data) {
    float64 i_fData[] = {data};  // 初始化数组并赋值
    int32 i_iErrorCode = 0;

    if (!m_readWrite) {
        i_iErrorCode = DAQmxWriteAnalogF64(m_NIDAQParameters.m_oTaskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, i_fData, NULL, NULL);
        m_readWrite = true;
    }
    if (i_iErrorCode != 0) {
        char i_chErrBuff[2048] = { 0 };
        DAQmxGetExtendedErrorInfo(i_chErrBuff, sizeof(i_chErrBuff));
        std::cerr << "Error writing data: " << i_chErrBuff << std::endl;
        return i_iErrorCode;
    }
    QThread::msleep(qMax(1, static_cast<int>(1.0 / m_NIDAQParameters.m_iTime * 500)));
    return 0;  // 成功返回 0 或其他适当值
}

QVector<float64> NIDAQCommunication::LoadDataFromExcel(const QString& filePath) {
    QXlsx::Document xlsx(filePath);
    if (!xlsx.selectSheet("Sheet1")) {
        std::cerr << "Failed to select sheet 'Sheet1'" << std::endl;
        return dataContainer;  // 返回空容器表示读取失败
    }

    int row = 1;
    while (true) {
        QVariant value = xlsx.read(row, 1);
        if (value.isNull()) {
            break;  // 读取完所有数据
        }
        dataContainer.append(value.toFloat());
        row++;
    }

    return dataContainer;
}

void NIDAQCommunication::ProcessExcelAndOutput(const QString& filePath) {
    m_stopFlag = false;

    if (dataContainer.isEmpty()) {
        std::cerr << "No data loaded from Excel file." << std::endl;
        return;
    }

    int row = 0;  // 从容器开始
    while (!m_stopFlag) {  // 检查停止标志
        if (row >= dataContainer.size()) {
            if (m_iLoops < 0) {
                std::cout << "Reached end of data at row: " << row << ". Restarting from row 1." << std::endl;
                row = 0;
                continue;
            }
            else if (m_iLoops > 0) {
                m_iLoops--;
                if (m_iLoops == 0) {
                    std::cout << "Reached the last loop. Stopping." << std::endl;
                    m_stopFlag = true; 
                }
                else {
                    row = 0;
                    continue;
                }
            }
        }
        if (!(m_iLoops == 0)) {
            float64 data = dataContainer[row];
            //float64 processedData = data /*/ m_NIDAQParameters.m_iType*/;
            //m_mtxBuffer.push_back(processedData);
            int32 errorCode = OutputData(data);
                if (errorCode != 0) {
                    std::cerr << "Error occurred during data output at row: " << row << std::endl;
                    break;  // 写入失败，退出循环
                }
        }
        ++row;  // 读取下一行
        QThread::msleep(qMax(1, static_cast<int>(1.0 / m_NIDAQParameters.m_iTime * 500)));
    }

    if (m_stopFlag) {
        std::cout << "Process stopped by user." << std::endl;
    }
}

void NIDAQCommunication::StopAcquisition() {
    if (m_isAcquiring) {
        m_stopInput = true;
        m_running = false;
        if (!m_stopFlag) {
            // 停止并清理 outputThread
            if (outputThread && outputThread->isRunning()) {
                m_stopFlag = true;
                outputThread->quit();
                outputThread->wait();
            }
            outputThread = nullptr;
        }
        else {
            m_stopFlag = true;
        }

        // 停止和清除任务 m_oTaskHandle
        if (m_NIDAQParameters.m_oTaskHandle != 0) {
            int32 stop = DAQmxStopTask(m_NIDAQParameters.m_oTaskHandle);
            int32 clear = DAQmxClearTask(m_NIDAQParameters.m_oTaskHandle);

            // 重置任务句柄
            m_NIDAQParameters.m_oTaskHandle = 0;
        }

        // 停止主任务
        if (m_thread && m_thread->isRunning()) {
            m_thread->quit();
            m_thread->wait();
        }
        m_thread = nullptr;

        // 停止和清除 NIDAQ 任务
        int32 stop = DAQmxStopTask(m_NIDAQParameters.m_iTaskHandle);
        int32 clear = DAQmxClearTask(m_NIDAQParameters.m_iTaskHandle);

        // 重置任务句柄
        m_NIDAQParameters.m_iTaskHandle = 0;
        m_isAcquiring = false;
    }
}

bool NIDAQCommunication::PulseCom() {
    // 检查是否已初始化
    if (!m_isInitialized) {
        std::cerr << "Error: Device not initialized!" << std::endl;
        return false;
    }

    // 检查任务句柄是否有效
    if (m_NIDAQParameters.m_iTaskHandle == nullptr) {
        std::cerr << "Error: Invalid task handle!" << std::endl;
        return false;
    }

    // 配置计数器输出参数
    const double frequency = 10.0;  // 频率 (Hz)
    const double dutyCycle = 0.5;   // 占空比 (50%)
    const double initialDelay = 0.0;  // 初始延迟 (秒)
    const int pulseCount = 10;         // 生成脉冲数量

    // 配置计数器输出为频率模式，设置初始延迟、频率和占空比
    int32 status = DAQmxCreateCOPulseChanFreq(m_NIDAQParameters.m_iTaskHandle, "Dev2/ctr0", "",
        DAQmx_Val_Hz, DAQmx_Val_Low,
        initialDelay, frequency, dutyCycle);
    if (status != 0) {
        std::cerr << "Error configuring pulse output: " << status << std::endl;
        return false;
    }

    // 配置隐式定时，生成指定数量的脉冲
    status = DAQmxCfgImplicitTiming(m_NIDAQParameters.m_iTaskHandle, DAQmx_Val_FiniteSamps, pulseCount);
    if (status != 0) {
        std::cerr << "Error configuring implicit timing: " << status << std::endl;
        return false;
    }

    // 启动任务
    status = DAQmxStartTask(m_NIDAQParameters.m_iTaskHandle);
    if (status != 0) {
        std::cerr << "Error starting task: " << status << std::endl;
        return false;
    }

    return true;
}

// 从data中读取数据
float64  NIDAQCommunication::GetData() {
    return m_NIDAQParameters.m_fData[0];
}

// 获取设备状态
std::string NIDAQCommunication::GetDeviceStatus() const {
    if (!m_isInitialized) {
        return "Device not initialized.";
    }
    if (m_isAcquiring) {
        return "Acquiring data.";
    }
    return "Idle.";
}

// 重置设备
bool NIDAQCommunication::ResetDevice() {
    // 重置设备代码
    m_isInitialized = false;
    return true;
}

void NIDAQCommunication::DeleteXlsx() {
    m_xlsx.deleteSheet("Sheet1");
    m_xlsx.addSheet("New");
    // 最后保存
    m_xlsx.save();
    // 删除并恢复原工作表
    m_xlsx.deleteSheet("New");
    m_xlsx.addSheet("Sheet1");

    // 再次保存
    m_xlsx.save();
}

void NIDAQCommunication::StopProcess()
{
    m_thread = nullptr;       // 重置线程指针
    m_isAcquiring = false;    // 更新状态
    //m_stopFlag = true;
}

void NIDAQCommunication::SetTime(float64 time)
{
    m_NIDAQParameters.m_iTime = time;
}

void NIDAQCommunication::SetType()
{
    m_NIDAQParameters.m_iType = (m_NIDAQParameters.m_fMaxVolt - m_NIDAQParameters.m_fMinVolt) / 
        (m_NIDAQParameters.m_fMaxValue - m_NIDAQParameters.m_fMinValue); // 默认系数为1.0
}

void NIDAQCommunication::SetInputChannel(std::string channel) {
    std::string result = "Dev2/";
    result += channel;
    m_NIDAQParameters.m_inputChannel = result;
}

void NIDAQCommunication::SetOutputChannel(std::string channel)
{
    std::string result = "Dev2/";
    result += channel;
    m_NIDAQParameters.m_outputChannel = result;
}

void NIDAQCommunication::SetMinValue(float64 minvalue)
{
    m_NIDAQParameters.m_fMinValue = minvalue;
    SetType();;
}

void NIDAQCommunication::SetMaxValue(float64 maxvalue)
{
    m_NIDAQParameters.m_fMaxValue = maxvalue;
    SetType();;
}

void NIDAQCommunication::SetMinVolt(float64 minvolt)
{
    m_NIDAQParameters.m_fMinVolt = minvolt;
    SetType();
}

void NIDAQCommunication::SetMaxVolt(float64 maxvolt)
{
    m_NIDAQParameters.m_fMaxVolt = maxvolt;
    SetType();
}

void NIDAQCommunication::SetLoops(int loops) {
    m_iLoops = loops;
}

float64 NIDAQCommunication::GetTime() const { return m_NIDAQParameters.m_iTime; }
float64 NIDAQCommunication::GetMinValue() const { return m_NIDAQParameters.m_fMinValue; }
float64 NIDAQCommunication::GetMaxValue() const { return m_NIDAQParameters.m_fMaxValue; }
float64 NIDAQCommunication::GetMinVolt() const { return m_NIDAQParameters.m_fMinVolt; }
float64 NIDAQCommunication::GetMaxVolt() const { return m_NIDAQParameters.m_fMaxVolt; }
std::string NIDAQCommunication::GetInputChannel() const {
    size_t pos = m_NIDAQParameters.m_inputChannel.find("/ai");
    if (pos != std::string::npos) {
        // 截取 "ao" 后面的数字部分
        std::string channel = m_NIDAQParameters.m_inputChannel.substr(pos + 3);
        return channel;
    }
    return "";  // 如果没有找到，返回空字符串
}
int NIDAQCommunication::GetInputChannelIndex() const {
    std::string channel = GetInputChannel();
    if (!channel.empty()) {
        // 转换为数字获取索引值
        try {
            return std::stoi(channel);
        }
        catch (const std::invalid_argument&) {
            // 如果转换失败，返回 -1 或其他默认值
            return -1;
        }
    }
    return -1;
}
std::string NIDAQCommunication::GetOutputChannel() const { 
    size_t pos = m_NIDAQParameters.m_outputChannel.find("/ao");
    if (pos != std::string::npos) {
        // 截取 "ao" 后面的数字部分
        std::string channel = m_NIDAQParameters.m_outputChannel.substr(pos + 3);
        return channel;
    }
    return "";  // 如果没有找到，返回空字符串
}
int NIDAQCommunication::GetOutputChannelIndex() const {
    std::string channel = GetOutputChannel();
    if (!channel.empty()) {
        // 转换为数字获取索引值
        try {
            return std::stoi(channel);
        }
        catch (const std::invalid_argument&) {
            // 如果转换失败，返回 -1 或其他默认值
            return -1;
        }
    }
    return -1;
}
bool NIDAQCommunication::IsAcquiring() const {
    return m_isAcquiring;  // 使用已有的 m_isAcquiring 成员变量
}
