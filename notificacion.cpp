#include "notificacion.h"
#include<QStringList>
#include<QDateTime>

//esta clase nos serivara para guardar quien es el usuario destino, el que recibira la notificacion
//para saber quien es el usuario relacionado(el que envio la solicitud o respondio).
//El tipo (solicitud, respuesta).

//    El mensaje a mostrar.

//    El estado (nueva, vista) para saber si ya se mostro

Notificacion::Notificacion(QString destino,QString relacionado,QString tipo,QString mensaje,QString estado)
    : destino(destino),relacionado(relacionado),tipo(tipo),mensaje(mensaje),estado(estado)

{

    QDateTime ahora=QDateTime::currentDateTime();
    fecha=ahora.toString("yyyy-MM-dd");
    hora=ahora.toString("HH:mm");

}


QString Notificacion::getDestino()const{return destino;}
QString Notificacion::getRelacionado()const{return relacionado;}
QString Notificacion::getTipo()const{return tipo;}
QString Notificacion::getMensaje()const{return mensaje;}
QString Notificacion::getEstado()const{return estado;}
QString Notificacion::getFecha()const{return fecha;}
QString Notificacion::getHora()const{return hora;}


void Notificacion::setEstado(const QString &nuevoEstado)
{

    estado=nuevoEstado;

}

QString Notificacion::serializar()const
{

    return destino+"|"+relacionado+"|"+tipo+"|"+mensaje+"|"+estado+"|"+fecha+"|"+hora;

}

Notificacion Notificacion::deserializar(const QString &linea)
{

    QStringList partes=linea.split("|");
    if(partes.size()==7)
    {
        Notificacion n(partes[0], partes[1], partes[2], partes[3], partes[4]);
        n.fecha=partes[5];
        n.hora=partes[6];
        return n;
    }
    return Notificacion();//invalidaaaa

}
