#include "registro.h"
#include"login.h"
#include"inicio.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Inicio i;
    i.show();

    return a.exec();
}
