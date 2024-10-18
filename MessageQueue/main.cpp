#include "CommModule.h"
#include "view/NetworkDlg.h"
#include "view/SerialDlg.h"
#include "view/SimulateSetDlg.h"
#include "view/PulseSetDlg.h"
#include "view/MessageQueues.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//CommModule w;
	//NetworkDlg w;
	//SerialDlg w;
	//SimulateSetDlg w;
	//PulseSetDlg w;
	MessageQueues w;
	w.show();
	return a.exec();
}
