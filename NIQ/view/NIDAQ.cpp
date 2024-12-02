#include "NIDAQ.h"
#include "NIDAQCommunication.h"
#include <QDebug>

NIDAQ::NIDAQ(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_NIDAQCommunication = new NIDAQCommunication();
    // 加载配置
    m_NIDAQCommunication->LoadParInfo("./NIIni.bin");

    // 显示加载的配置
    ui.TimeLineEdit->setText(QString::number(m_NIDAQCommunication->GetTime()));
    ui.editMinValue->setValue(m_NIDAQCommunication->GetMinValue());
    ui.editMaxValue->setValue(m_NIDAQCommunication->GetMaxValue());
    ui.editMinVolt->setValue(m_NIDAQCommunication->GetMinVolt());
    ui.editMaxVolt->setValue(m_NIDAQCommunication->GetMaxVolt());
    ui.InputComboBox->setCurrentIndex(m_NIDAQCommunication->GetInputChannelIndex());
    ui.OutputComboBox->setCurrentIndex(m_NIDAQCommunication->GetOutputChannelIndex());
    Init();
    SlotConnection();
}

NIDAQ::~NIDAQ()
{
    m_NIDAQCommunication->SaveParInfo("./NIIni.bin");
}

void NIDAQ::Init() {

}

void NIDAQ::SlotConnection() {
    ui.StartButton->setFocus();

    //connect(ui.OutputButton, &QPushButton::clicked, this, &NIDAQ::OnOutputButtonClicked);
    //connect(ui.InputButton, &QPushButton::clicked, this, &NIDAQ::OnInputButtonClicked);
    connect(ui.StartButton, &QPushButton::clicked, this, &NIDAQ::OnStartButtonClicked);
    connect(m_NIDAQCommunication, &NIDAQCommunication::DataAcquired, this, &NIDAQ::UpdateInput);
    connect(ui.StopButton,&QPushButton::clicked,this,&NIDAQ::OnStopButtonClicked);
    connect(ui.HardTriggeredButton,&QPushButton::clicked,this,&NIDAQ::OnHardTriggeredButtonClicked);
    connect(ui.TimeLineEdit ,& QLineEdit::editingFinished, this, &NIDAQ::SetTime);
    //connect(ui.TypeLineEdit,& QLineEdit::editingFinished, this, &NIDAQ::SetType);
    connect(ui.editMinVolt, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &NIDAQ::SetMinVolt);
    connect(ui.editMaxVolt, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &NIDAQ::SetMaxVolt);
    connect(ui.editMinValue, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &NIDAQ::SetMinvalue);
    connect(ui.editMaxValue, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &NIDAQ::SetMaxvalue);
    connect(ui.LoopsLineEdit,&QLineEdit::editingFinished,this,&NIDAQ::SetLoops);
    connect(ui.InputComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &NIDAQ::SetInputChannel);   
    connect(ui.OutputComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &NIDAQ::SetOutputChannel);
    //connect(ui.InputComboBox, &QComboBox::currentIndexChanged, this, &NIDAQ::SetInputChannel);
    //connect(ui.OutputComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(SetOutputChannel(const QString)));
}

void NIDAQ::OnStartButtonClicked()
{
    // 读取数据
    if (!m_load) {
        m_NIDAQCommunication->LoadDataFromExcel("NI-Input.xlsx");
        m_load = true;
    }
    m_NIDAQCommunication->DeleteXlsx();
    // 先初始化输出通道
    const char* i_NIDAQ = "NIDAQ1";
    bool outputInitSuccess = m_NIDAQCommunication->oInitialize(std::string(i_NIDAQ));

    if (outputInitSuccess) {
        // 配置输出通道
        m_NIDAQCommunication->OutputConfigureChannel();
        // 启动数据输出采集
        m_NIDAQCommunication->StartOutputAcquisition();
        ui.OutputTextEdit->setPlainText(QString::fromLocal8Bit("已开始数据输出。"));
    }
    else {
        qWarning() << "Output initialization failed!";
        return;
    }

    // 接着初始化输入通道
    const char* o_NIDAQ = "NIDAQ2";
    bool inputInitResult = m_NIDAQCommunication->Initialize(std::string(o_NIDAQ));

    if (inputInitResult) {
        // 配置输入通道
        m_NIDAQCommunication->InputConfigureChannel();
        // 启动数据输入采集
        m_NIDAQCommunication->StartAcquisition();
        ui.OutputTextEdit->appendPlainText(QString::fromLocal8Bit("已开始数据输入。"));
    }
    else {
        // 处理输入初始化失败的情况
        qWarning() << "Input initialization failed!";
    }
}

void NIDAQ::OnOutputButtonClicked() {

    //if (m_NIDAQCommunication->IsAcquiring()) {
    //    m_NIDAQCommunication->StopAcquisition();
    //}
    const char* i_iNIDAQ = "NIDAQ1";
    bool initSuccess =m_NIDAQCommunication->oInitialize(std::string(i_iNIDAQ));

    if (initSuccess) {
        m_NIDAQCommunication->OutputConfigureChannel();
        // 启动新的数据输入
        m_NIDAQCommunication->StartOutputAcquisition();
        ui.OutputTextEdit->setPlainText(QString::fromLocal8Bit("已开始数据输出。"));
    }
}

void NIDAQ::OnInputButtonClicked() {
    //// 如果之前有采集，先停止
    //if (m_NIDAQCommunication->IsAcquiring()) {
    //    m_NIDAQCommunication->StopAcquisition();
    //}

    // 重新初始化
    const char* i_oNIDAQ = "NIDAQ2";
    bool initResult = m_NIDAQCommunication->Initialize(std::string(i_oNIDAQ));

    if (initResult) {
        m_NIDAQCommunication->InputConfigureChannel();
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        // 处理初始化失败的情况
        qWarning() << "NIDAQ initialization failed";
    }
}

void NIDAQ::UpdateInput(float64* data, int count) {
    if (m_stop) {
        qDebug() << "The task has stopped.";
        return;
    }
    if (data && count > 0) {
        QStringList dataList;
        for (int i = 0; i < count; ++i) {
            QString line = QString::fromUtf8(u8"通道ai1的电压值：") + QString::number(data[i]);
            dataList.append(line);
        }
        ui.InputTextEdit->setPlainText(dataList.join("\n"));
    }
    else {
        ui.InputTextEdit->setPlainText("No data available.");
    }
}

//void NIDAQ::Output() {
//    m_NIDAQCommunication.ConfigureChannel();
//    m_NIDAQCommunication.StartAcquisition();
//    float64 i_fData[1000];
//    i_fData[0] = m_NIDAQCommunication.GetData();
//    QString i_newText = QString::number(i_fData[0]); // 转换浮点数为字符串
//    ui.OutputTextEdit->setPlainText(i_newText);      // 设置到文本框中
//}

void NIDAQ::OnHardTriggeredButtonClicked() {
    m_NIDAQCommunication->PulseCom();
}

void NIDAQ::SetTime()
{
    QString i_text = ui.TimeLineEdit->text();
    bool i_bOk;
    double i_dtimeValue = i_text.toDouble(&i_bOk);

    if (i_bOk) {
        m_NIDAQCommunication->StopAcquisition();
        m_NIDAQCommunication->SetTime(i_dtimeValue);
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        qWarning() << "Invalid input: unable to convert to double.";
    }
}

void NIDAQ::SetType()
{
    //QString i_text = ui.TypeLineEdit->text();
    //bool i_bOk;
    //double i_dTypeValue = i_text.toDouble(&i_bOk);

    //if (i_bOk) {
    //    m_NIDAQCommunication->StopAcquisition();
    //    m_NIDAQCommunication->SetType(i_dTypeValue);
    //    m_NIDAQCommunication->StartAcquisition();
    //}
    //else {
    //    qWarning() << "Invalid input: unable to convert to double.";
    //}
}

void NIDAQ::SetInputChannel( int index)
{
    QString selectedText = ui.InputComboBox->itemText(index);
    std::string stdText = selectedText.toStdString();
    //m_NIDAQCommunication->StopAcquisition();
    m_NIDAQCommunication->SetInputChannel(stdText);
    m_NIDAQCommunication->InputConfigureChannel();
    //m_NIDAQCommunication->StartAcquisition();
}

void NIDAQ::SetOutputChannel(int index)
{
    QString selectedText = ui.OutputComboBox->itemText(index);
    std::string stdText = selectedText.toStdString();

    //m_NIDAQCommunication->StopAcquisition();
    m_NIDAQCommunication->SetOutputChannel(stdText);
    m_NIDAQCommunication->OutputConfigureChannel();
    //m_NIDAQCommunication->StartAcquisition();
}

void NIDAQ::SetMinvalue()
{
    QString i_text = ui.editMinValue->text();
    bool i_bOk;
    double i_dMinValue = i_text.toDouble(&i_bOk);

    if (i_bOk) {
        m_NIDAQCommunication->StopAcquisition();
        m_NIDAQCommunication->SetMinValue(i_dMinValue);
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        qWarning() << "Invalid input: unable to convert to double.";
    }
}

void NIDAQ::SetMaxvalue()
{
    QString i_text = ui.editMaxValue->text();
    bool i_bOk;
    double i_dMaxValue = i_text.toDouble(&i_bOk);

    if (i_bOk) {
        m_NIDAQCommunication->StopAcquisition();
        m_NIDAQCommunication->SetMaxValue(i_dMaxValue);
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        qWarning() << "Invalid input: unable to convert to double.";
    }
}

void NIDAQ::SetMinVolt()
{
    QString i_text = ui.editMinVolt->text();
    bool i_bOk;
    double i_dMinVolt = i_text.toDouble(&i_bOk);

    if (i_bOk) {
        m_NIDAQCommunication->StopAcquisition();
        m_NIDAQCommunication->SetMinVolt(i_dMinVolt);
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        qWarning() << "Invalid input: unable to convert to double.";
    }
}

void NIDAQ::SetMaxVolt()
{
    QString i_text = ui.editMaxVolt->text();
    bool i_bOk;
    double i_dMaxVolt = i_text.toDouble(&i_bOk);

    if (i_bOk) {
        m_NIDAQCommunication->StopAcquisition();
        m_NIDAQCommunication->SetMaxVolt(i_dMaxVolt);
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        qWarning() << "Invalid input: unable to convert to double.";
    }
}

void NIDAQ::SetLoops() {
    QString i_text = ui.LoopsLineEdit->text();
    bool i_bOk;
    int i_dtimeValue = i_text.toInt(&i_bOk);

    if (i_bOk) {
        m_NIDAQCommunication->StopAcquisition();
        m_NIDAQCommunication->SetLoops(i_dtimeValue);
        m_NIDAQCommunication->StartAcquisition();
    }
    else {
        qWarning() << "Invalid input: unable to convert to int.";
    }
}

void NIDAQ::OnStopButtonClicked() {
    m_stop = false;
    m_NIDAQCommunication->StopAcquisition();
    ui.OutputTextEdit->setPlainText(QString::fromLocal8Bit("已停止数据输出！"));
    //m_NIDAQCommunication->StopProcess();
}