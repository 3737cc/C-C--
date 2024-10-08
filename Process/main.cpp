#include "Process.h"
#include "ProcessB.h"
#include "SharedMemory.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Process pA;
	ProcessB pB;
	pA.show();
	pB.show();
	return a.exec();
}
