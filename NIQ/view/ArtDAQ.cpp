#include "ArtDAQ.h"
#include "ArtCommunication.h"
#define MI 3.1415926

ArtDAQ::ArtDAQ(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
    m_artCommunication = new ArtCommunication();
    Init();
    SlotConnection();
}

ArtDAQ::~ArtDAQ()
{
}

void ArtDAQ::Init()
{
	m_artCommunication->Initialize();
}

void ArtDAQ::SlotConnection()
{
	connect(ui.OutputButton, &QPushButton::clicked, this, &ArtDAQ::OnOutputButtonClicked);
	connect(ui.InputButton, &QPushButton::clicked, this, &ArtDAQ::OnInputButtonClecked);
	connect(ui.StopButton, &QPushButton::clicked, this, &ArtDAQ::OnStopButtonClecked);
}

void ArtDAQ::OnOutputButtonClicked() {
    // 创建一个用于存储电压数据的 vector
    std::vector<double> voltageData;

    // 生成一个正弦波电压数据序列
    const int MAX_DATA_POINTS = 1000;  // 假设你想要1000个数据点
    for (int i = 0; i < MAX_DATA_POINTS; ++i) {
        // 正弦波电压公式：幅度9.95，周期2PI
        double voltage = 9.95 * sin(static_cast<double>(i) * 2.0 * MI / MAX_DATA_POINTS);
        voltageData.push_back(voltage);
    }

    // 调用 WriteVoltageData 函数，传入生成的电压数据
    if (!m_artCommunication->WriteVoltageData(voltageData)) {
        std::cerr << "Failed to write voltage data!" << std::endl;
    }
    else {
        std::cout << "Voltage data written successfully." << std::endl;
    }
}

void ArtDAQ::OnInputButtonClecked() {
    std::vector<double> i_voltageData;  // 用于存储读取的电压数据
    bool success = m_artCommunication->ReadVoltageData(i_voltageData);

    if (success) {
        QString voltageText;
        for (const auto& voltage : i_voltageData) {
            voltageText += QString::number(voltage) + "\n";
        }

        ui.InputTextEdit->setPlainText(voltageText);
    }
    else {
        ui.InputTextEdit->setPlainText("Failed to read voltage data.");
    }
}

void ArtDAQ::OnStopButtonClecked() {

}
