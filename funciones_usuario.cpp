#include "funciones_usuario.h"
#include<QFile>
#include<QTextStream>



QList<Usuario> CargarUsuarios()
{

    QList<Usuario>lista;
    QFile archivo("usuarios.txt");

    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&archivo);
        while(!in.atEnd())
        {

            QString linea=in.readLine();
            QStringList partes=linea.split("|");

            if(partes.size()==9)
            {

                Usuario u(
                    partes[0],partes[1],partes[2],partes[3],partes[4].toInt(),partes[5],partes[6],partes[7],
                    partes[8]=="1");

                lista.append(u);

            }

        }
        archivo.close();

    }
    return lista;

}
