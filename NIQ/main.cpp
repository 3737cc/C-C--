#include "NIDAQ.h"
#include "ArtDAQ.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    NIDAQ w;
    //ArtDAQ w;
    w.show();
    return a.exec();
}
