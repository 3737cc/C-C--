#ifndef ArtDAQ_H
#define ArtDAQ_H

#include <QMainWindow>
#include "ui_ArtDAQ.h"

class ArtCommunication;
class ArtDAQ : public QMainWindow
{
	Q_OBJECT

public:
	ArtDAQ(QWidget *parent = nullptr);
	~ArtDAQ();

	void Init();
	void SlotConnection();
	void OnOutputButtonClicked();
	void OnInputButtonClecked();
	void OnStopButtonClecked();

private:
	Ui::ArtDAQClass ui;
	ArtCommunication* m_artCommunication;
};

#endif
