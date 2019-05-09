#include "getufile.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GetUFile w;
    return a.exec();
}
