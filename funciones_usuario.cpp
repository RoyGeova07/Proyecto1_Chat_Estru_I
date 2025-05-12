#include "funciones_usuario.h"
#include<QFile>
#include<QTextStream>
#include<QSet>
#include<QString>
#include<QDir>
#include<QCoreApplication>
#include<QStringList>

//aqui el dir proporciona acceso a las estructuras de directorios y sus contenidos,
//asi como a la informacion del sistema de archivos

QString ObtenerRutaUsuarios()
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();  //aqui sube una carpeta (salir de /debug o /release)

    QDir carpetaDatos(dir.filePath("datos"));
    if(!carpetaDatos.exists())
    {

        carpetaDatos.mkpath(".");  //aqui crea la carpeta si no existe

    }

    QString ruta=carpetaDatos.filePath("usuarios.txt");
    return ruta;
}
QList<Usuario> CargarUsuarios()
{

    QList<Usuario>lista;
    QFile archivo(ObtenerRutaUsuarios());

    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&archivo);
        while(!in.atEnd())
        {

            QString linea=in.readLine();
            QStringList partes=linea.split("|");

            if(partes.size()==9)
            {

                Usuario u(//usuario[1],nombre[2],correo[3],contra[4],       edad[5],pregunta[6], respuesta]7]
                    partes[0],partes[1],partes[2],partes[3],partes[4].toInt(),partes[5],partes[6],partes[7],
                    partes[8]=="1");//conectado[8]

                lista.append(u);

            }

        }
        archivo.close();

    }
    return lista;

}

void GuardarUsuarios(const QList<Usuario>&usuarios)
{

    QFile archivo(ObtenerRutaUsuarios());

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

QString RutaOnline()
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();  //aqui sube una carpeta (salir de /debug o /release)

    QDir carpetaDatos(dir.filePath("activo"));
    if(!carpetaDatos.exists())
    {

        carpetaDatos.mkpath(".");  //aqui crea la carpeta si no existe

    }

    QString ruta=carpetaDatos.filePath("usuarios_online.txt");
    return ruta;
}


//aqui esta funcion, lee el contenido completo
//verifica que no exista ya este usuario
//se mueve al final del archivo seek(archivo.size())) para no sobreescribir.
//usa QTextStream diferente para escribir.
void MarcarUsuarioComoConectado(const QString &usuario)
{

    QFile archivo(RutaOnline());
    if(!archivo.open(QIODevice::ReadWrite|QIODevice::Text))return;

    QTextStream lector(&archivo);
    QString contenido=lector.readAll();


    if(!contenido.contains(usuario+"\n"))
    {

        archivo.seek(archivo.size());//aqui va al final del archivo
        QTextStream escritor(&archivo);//aqui se crea un nuevo qtextstream para escribir
        escritor<<usuario<<"\n";

    }
    archivo.close();

}

void MarcarUsuarioComoDesconectado(const QString &usuario)
{

    QFile archivo(RutaOnline());
    if(!archivo.open(QIODevice::ReadOnly|QIODevice::Text))return;

    QStringList usuarios;
    QTextStream in(&archivo);
    while(!in.atEnd())
    {

        QString linea=in.readLine().trimmed();
        if(linea!=usuario&&!linea.isEmpty())
        {

            usuarios<<linea;

        }

    }
    archivo.close();

    if(archivo.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
    {

        QTextStream out(&archivo);
        for(const QString &u:usuarios)
            out<<u<<"\n";
        archivo.close();

    }

}

bool Estaconectado(const QString &usuario)
{

    QFile archivo(RutaOnline());
    if(!archivo.open(QIODevice::ReadOnly|QIODevice::Text))return false;

    QTextStream in(&archivo);
    while(!in.atEnd())
    {

        if(in.readLine().trimmed()==usuario)
            return true;

    }
    return false;

}

void LimpiarUsuariosOnlineSiEstaVacio()
{

    QFile archivo(RutaOnline());
    if(!archivo.exists())return;

    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream limpia(&archivo);
        bool Estavacio=false;

        while(!limpia.atEnd())
        {

            QString linea=limpia.readLine().trimmed();
            if(!linea.isEmpty())
            {

                Estavacio=false;
                break;

            }

        }
        archivo.close();

        if(Estavacio)
        {

            archivo.remove();//aqui elimina el archivo si solo esta vacio

        }

    }

}

void CrearCarpetaConversacion(const QString &u1,const QString &u2)
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    QDir carpetaMensajes(dir.filePath("conversaciones"));
    if(!carpetaMensajes.exists())
    {

        carpetaMensajes.mkpath(".");

    }

    //aqui se ordena alfabeticamente los nombres para unificar el archivo
    QString nombre1=u1.toLower();
    QString nombre2=u2.toLower();
    if(nombre1>nombre2)std::swap(nombre1,nombre2);

    QString rutaArchivo=carpetaMensajes.filePath(nombre1+"_"+nombre2+".txt");

    QFile archivo(rutaArchivo);
    if(!archivo.exists())
    {

        archivo.open(QIODevice::WriteOnly);
        archivo.close();

    }

}
QStringList CargarMensajeDesdeArchivo(const QString &u1, const QString &u2)
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();  // salir de /debug o /release
    QDir carpeta(dir.filePath("conversaciones"));

    QString nombre1=u1.toLower();
    QString nombre2=u2.toLower();
    if(nombre1>nombre2) std::swap(nombre1,nombre2);

    QString rutaArchivo=carpeta.filePath(nombre1 + "_" + nombre2 + ".txt");

    QStringList mensajes;
    QFile archivo(rutaArchivo);
    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&archivo);
        while(!in.atEnd())
        {

            QString linea=in.readLine();
            mensajes.append(linea);

        }
        archivo.close();

    }
    return mensajes;

}

