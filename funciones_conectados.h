#ifndef FUNCIONES_CONECTADOS_H
#define FUNCIONES_CONECTADOS_H

#include <QFile>
#include <QTextStream>
#include <QSet>
#include <QStringList>

inline void AgregarUsuarioConectado(const QString &usuario)
{
    QFile archivo("conectados.txt");
    QSet<QString> usuarios;

    if (archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&archivo);
        while (!in.atEnd())
            usuarios.insert(in.readLine());
        archivo.close();

    }

    if (!usuarios.contains(usuario))
    {

        if(archivo.open(QIODevice::Append|QIODevice::Text))
        {

            QTextStream out(&archivo);
            out <<usuario<<"\n";
            archivo.close();

        }

    }
}

inline void EliminarUsuarioConectado(const QString &usuario) {
    QFile archivo("conectados.txt");

    //aqui declara una lista de cadenas lineas que almacenara todas las lineas excepto el usuario que se quiere eliminar
    QStringList lineas;

    //Abre el archivo en modo lectura de texto.

    // Usa QTextStream para leer linea por linea.

    //Si la linea NO es el usuario que queremos eliminar, la guarda en la lista lineas.

    //Cierra el archivo.

    if (archivo.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream in(&archivo);
        while (!in.atEnd())
        {

            QString linea=in.readLine();
            if (linea!=usuario)
                lineas<<linea;

        }
        archivo.close();
    }

    //q hace el truncate?
    //este vacia completamente el archivo cuando se abre, elimina todo el contenido anterior
    //Queremos reescribir el archivo desde cero sin el usuario eliminado
    //Asi que uso Truncate para que el archivo quede vacio antes de escribir las nuevas lineas

    //aqui se cream un flujo de salida (QTextStream out) para escribir en el archivo ya vacio

    //aqui se escribe cada linea que si se debe conservar es decir, todas excepto el usuario eliminado

    //cerramos el archivo

    if (archivo.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
    {

        QTextStream out(&archivo);
        for(const QString &linea : lineas)
            out <<linea<<"\n";
        archivo.close();

    }
}

//en esta funcion abre el archivo, recorre cada linea y compara con el nombre del usuario, si lo encuentra, retorna true

inline bool EstaConectado(const QString &usuario)
{

    QFile archivo("conectados.txt");
    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            if (in.readLine()==usuario)
                return true;
        }
        archivo.close();
    }
    return false;
}

#endif // FUNCIONES_CONECTADOS_H
