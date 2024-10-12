#include "PipeB.h"

PipeB::PipeB(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.readButton, &QPushButton::clicked, this, &PipeB::onReadButtonClicked);
}

PipeB::~PipeB()
{
}

void PipeB::onReadButtonClicked() {
	QByteArray data = m_process->readLine();
	QString receivedData = QString::fromUtf8(data).trimmed();
	ui.valueOutput->setText(receivedData);
}