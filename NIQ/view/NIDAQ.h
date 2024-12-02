#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NIDAQ.h"
#include "NIDAQWorker.h"

class NIDAQCommunication;
class NIDAQ : public QMainWindow
{
    Q_OBJECT

public:
    NIDAQ(QWidget *parent = nullptr);
    ~NIDAQ();

    void Init();
    void SlotConnection();
    void UpdateInput(float64* data, int count);
    void OnStopButtonClicked();
    void OnInputButtonClicked();    // 输出数据
    void OnOutputButtonClicked();   // 输出数据
    void OnStartButtonClicked();    // 开始同步输入输出
    void OnHardTriggeredButtonClicked();
    // 设置参数
    void SetTime();
    void SetType();
    void SetInputChannel(int index);
    void SetOutputChannel( int index);
    void SetMinvalue();
    void SetMaxvalue();    
    void SetMinVolt();
    void SetMaxVolt();
    void SetLoops();

private:
    Ui::NIDAQClass ui;
    NIDAQCommunication* m_NIDAQCommunication;
    bool m_stop = false;
    bool m_isOutputialized = false;
    bool m_isInputInitialized = false;
    bool m_load = false;
};
