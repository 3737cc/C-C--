#include "Process.h"
#include "ProcessB.h"
#include "MessageQueues.h"
#include "MessageQueuesB.h"
#include "Pipe.h"
#include "PipeB.h"
#include "SharedMemory.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//Pipe pA;
	//PipeB pB;
	//pA.show();
	//pB.show();
	MessageQueues m;
	//MessageQueuesB mb;
	m.show();
	//mb.show();
	//Process pA;
	//ProcessB pB;
	//pA.show();
	//pB.show();
	return a.exec();
}
