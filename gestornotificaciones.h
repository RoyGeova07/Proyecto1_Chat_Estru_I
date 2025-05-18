#ifndef GESTORNOTIFICACIONES_H
#define GESTORNOTIFICACIONES_H

#include"notificacion.h"
#include<QList>
#include<QString>
#include<QDir>
#include<QCoreApplication>

class GestorNotificaciones
{
public:
    GestorNotificaciones();
    QList<Notificacion>cargar(const QString &usuario);
    void guardar(const QString &usuario,const QList<Notificacion>&notificaciones);
    void MarcarComoVista(const QString &usuario,int indice);//para saber cuanod la leyoooo
    void eliminar(const QString &usuario,int indice);//para eliminar una
    void agregarnotificacion(const Notificacion &nueva);
    void MarcarComoVistaBuzon(const QString &usuario);

    void eliminarTodasLasNotificaciones(const QString& usuario);
    static void EliminarNotificacionesEntre(const QString &usuario1, const QString &usuario2);

private:

    QString ObtenerRutaArchivo(const QString &usuario);

    static QString RutaNotificaciones()
    {

        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();

        QDir carpeta(dir.filePath("notificaciones"));
        if(!carpeta.exists())
        {

            carpeta.mkpath(".");

        }
        return carpeta.path();

    }

};

#endif // GESTORNOTIFICACIONES_H
