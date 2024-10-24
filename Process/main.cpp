#include "Process.h"
#include "ProcessB.h"
#include "MessageQueues.h"
#include "MessageQueuesB.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	MessageQueues m;
	m.show();
	//MessageQueuesB mb;
	//mb.show();
	//Process pA;
	//ProcessB pB;
	//pA.show();
	//pB.show();
	return a.exec();
}
