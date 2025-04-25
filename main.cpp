#include "registro.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Registro r;
    r.show();

    return a.exec();
}
