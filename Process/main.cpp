#include "Process.h"
#include "SharedMemory.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	Process w;
	w.show();
	return a.exec();
}
