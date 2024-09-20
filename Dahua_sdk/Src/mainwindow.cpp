#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "CameraControl.h"

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	isConnected(false)
{
	ui->setupUi(this);

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	CameraControl cameraControl;
	if (isConnected) {
		cameraControl.connectCamera();
		isConnected = false;
	}
	else {
		cameraControl.disconnectCamera();
		isConnected = true;
	}
}

