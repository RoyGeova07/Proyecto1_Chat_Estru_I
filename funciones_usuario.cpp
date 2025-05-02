#include "funciones_usuario.h"
#include<QFile>
#include<QTextStream>
#include<QSet>
#include<QString>


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

void GuardarUsuarios(const QList<Usuario>&usuarios)
{

    QFile archivo("usuarios.txt");

    if(archivo.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        QTextStream out(&archivo);

        for(const Usuario &guardar:usuarios)
        {


            out<<guardar.getUsuario() << "|"<<guardar.getNombre()<<"|"<<guardar.getCorreo()<<"|"<<guardar.getContrasena()<<"|"
            <<guardar.getEdad()<<"|"<<guardar.getPregunta()<<"|"<<guardar.getRespuesta()<<"|"<<guardar.getRutaAvatar()<<"|"
            <<(guardar.estaConectado()?"1":"0")<<"\n";

        }
        archivo.close();

    }

}



