#include "Pipe.h"

Pipe::Pipe(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.writeButton, &QPushButton::clicked, this, &Pipe::onWriteButtonClicked);
	//m_processB.start("PipeB");
}

Pipe::~Pipe()
{
}

void Pipe::onWriteButtonClicked() {
	QString dataToWrite = ui.valueInput->text();
	if (m_processB.state() == QProcess::Running) {
		m_processB.write(dataToWrite.toUtf8() + "\n"); // 发送数据并换行
	}
	else {
		qDebug() << "Process B is not running.";
	}
}
