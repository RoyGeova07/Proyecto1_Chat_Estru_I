#include "gestornotificaciones.h"
#include<QFile>
#include<QTextStream>
#include<QList>


//que hara esta clase?
//aqui cada usuario tiene su archivo notificaciones/nombre.txt
//podra cargar,guardar,agregar,marcar como vist o eliminar notificaciones individualmente
//esto mantiene todo organizado y personalizado para cada cuenta

GestorNotificaciones::GestorNotificaciones() {}

QString GestorNotificaciones::ObtenerRutaArchivo(const QString &usuario)
{


    return RutaNotificaciones()+"/notificaciones_"+usuario+".txt";


}

QList<Notificacion> GestorNotificaciones::cargar(const QString &usuario)
{

    QList<Notificacion> lista;
    QFile archivo(ObtenerRutaArchivo(usuario));

    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&archivo);
        while(!in.atEnd())
        {

            QString linea=in.readLine();
            Notificacion noti=Notificacion::deserializar(linea);
            if(!noti.getDestino().isEmpty())
            {

                lista.append(noti);

            }

        }
        archivo.close();

    }
    return lista;

}

void GestorNotificaciones::guardar(const QString &usuario, const QList<Notificacion> &notificaciones)
{

    QFile archivo(ObtenerRutaArchivo(usuario));
    if(archivo.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        QTextStream out(&archivo);
        for(const Notificacion &no:notificaciones)
        {

            out<<no.serializar()<<"\n";

        }
        archivo.close();
    }

}
void GestorNotificaciones::agregarnotificacion(const Notificacion &nueva)
{
    QList<Notificacion> actuales = cargar(nueva.getDestino());

    if (nueva.getTipo() == "solicitud")
    {
        for (int i = actuales.size() - 1; i >= 0; --i)
        {
            if (actuales[i].getTipo() == "solicitud"&&actuales[i].getRelacionado() == nueva.getRelacionado())
            {
                QString estado = actuales[i].getEstado();

                if (actuales[i].getRelacionado() == nueva.getRelacionado() && actuales[i].getTipo() == "solicitud") {
                    actuales.removeAt(i);
                    break; //aqui solo se elimina la mas reciente
                }
            }
        }
    }

    actuales.append(nueva);
    guardar(nueva.getDestino(), actuales);
}
void GestorNotificaciones::MarcarComoVista(const QString &usuario, int indice)
{

    QList<Notificacion>lista=cargar(usuario);
    if(indice>=0&&indice<lista.size())
    {

        lista[indice].setEstado("vista");
        guardar(usuario,lista);

    }

}

void GestorNotificaciones::eliminar(const QString &usuario, int indice)
{

    QList<Notificacion>lista=cargar(usuario);
    if(indice>=0&&indice<lista.size())
    {

        lista.removeAt(indice);
        guardar(usuario,lista);

    }

}

void GestorNotificaciones::MarcarComoVistaBuzon(const QString &usuario)
{

    QList<Notificacion>lista=cargar(usuario);
    for(Notificacion &n:lista)
    {

        if((n.getTipo()=="solicitud"||n.getTipo()=="respuesta")&&n.getEstado()=="nueva")
        {

            n.setEstado("vista");

        }

    }
    guardar(usuario,lista);

}

void GestorNotificaciones::eliminarTodasLasNotificaciones(const QString &usuario)
{

    QFile archivo(ObtenerRutaArchivo(usuario));
    if (archivo.exists())
    {

        archivo.remove();

    }

}

void GestorNotificaciones::EliminarNotificacionesEntre(const QString &usuario1, const QString &usuario2) {
    QString rutaBase = RutaNotificaciones();

    QStringList archivos;
    archivos << QString("notificaciones_%1.txt").arg(usuario1)
             << QString("notificaciones_%1.txt").arg(usuario2);

    for (const QString &archivoNombre : archivos) {
        QFile archivo(QDir(rutaBase).filePath(archivoNombre));
        if (!archivo.exists()) continue;

        if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QStringList lineasNuevas;
            QTextStream in(&archivo);
            while (!in.atEnd()) {
                QString linea = in.readLine();
                QStringList partes = linea.split("|");
                if (partes.size() >= 5) {
                    QString remitente = partes[0];
                    QString receptor = partes[1];
                    QString tipo = partes[2];

                    // Solo conserva la línea si NO involucra a ambos usuarios
                    if (!((remitente == usuario1 && receptor == usuario2) ||
                          (remitente == usuario2 && receptor == usuario1))) {
                        lineasNuevas << linea;
                    }
                }
            }
            archivo.close();

            if (archivo.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream out(&archivo);
                for (const QString &l : lineasNuevas)
                    out << l << "\n";
                archivo.close();
            }
        }
    }
}
