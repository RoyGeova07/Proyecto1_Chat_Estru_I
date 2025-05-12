#ifndef FUNCIONES_USUARIO_H
#define FUNCIONES_USUARIO_H

#include "usuario.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QSet>
#include<QDir>
#include<QCoreApplication>
#include"Mensaje.h"
#include<QStringList>

QList<Usuario> CargarUsuarios(); // funcion libre, sin clase
void GuardarUsuarios(const QList<Usuario>&usuarios);

//la palabra clave extern no se usa para definir una funcion, sino para declararla en otro archivo
//y decirle al compilador que la implementacion esta en otro lugar.
extern QString ObtenerRutaUsuarios();
void MarcarUsuarioComoConectado(const QString &usuario);
void MarcarUsuarioComoDesconectado(const QString &usuario);
bool Estaconectado(const QString &usuario);
void LimpiarUsuariosOnlineSiEstaVacio();
void CrearCarpetaConversacion(const QString &u1,const QString &u2);

template<typename T>
void GuardarMensajeEnArchivo(const Mensaje<T> &m,const QString &u1,const QString &u2)
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QDir carpetaMensajes(dir.filePath("conversaciones"));

    QString nombre1=u1.toLower();
    QString nombre2=u2.toLower();
    if(nombre1>nombre2) std::swap(nombre1, nombre2);

    QString rutaArchivo = carpetaMensajes.filePath(nombre1 + "_" + nombre2 + ".txt");

    QFile archivo(rutaArchivo);
    if (archivo.open(QIODevice::Append | QIODevice::Text)) {

        QTextStream out(&archivo);
        out << "[" << m.getFecha().toString("yyyy-MM-dd HH:mm") << "] "
            << m.getEmisor() << " (" << m.tipoComoTexto() << "): "
            << m.getContenido() << "\n";
        archivo.close();

    }

}

QStringList CargarMensajeDesdeArchivo(const QString &u1, const QString &u2);

inline QString RutaSolicitudes()
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();//para que suba fuera de /debug o /release

    QDir carpeta(dir.filePath("soli"));
    if(!carpeta.exists())
    {

        carpeta.mkpath(".");

    }
    return carpeta.filePath("solicitudes.txt");

}
inline QString RutaContactos()
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();//para que suba fuera de /debug o /release

    QDir carpeta(dir.filePath("soli"));
    if(!carpeta.exists())
    {

        carpeta.mkpath(".");

    }
    return carpeta.filePath("contactos.txt");


}


#endif // FUNCIONES_USUARIO_H
