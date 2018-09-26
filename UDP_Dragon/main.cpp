//#--------------------------------------------------#
//#                                                  #
//#  Project created by: Johannes de Lange 23689293  #
//#                                                  #
//#--------------------------------------------------#

#include "dragon.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dragon w;
    w.show();

    return a.exec();
}
