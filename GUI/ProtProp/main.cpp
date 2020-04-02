#include "protprop.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProtProp w;
    w.show();
    return a.exec();
}
