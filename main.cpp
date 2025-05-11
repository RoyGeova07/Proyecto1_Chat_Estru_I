#include "registro.h"
#include"login.h"
#include"inicio.h"
#include"funciones_usuario.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LimpiarUsuariosOnlineSiEstaVacio();//se usa para evitar registros fantasmas y futuros errores

    Inicio i;
    i.show();

    return a.exec();
}
