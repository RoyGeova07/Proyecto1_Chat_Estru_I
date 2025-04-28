#include "solicitudcontacto.h"

SolicitudContacto::SolicitudContacto(QString remitente,QString destinatario,QString estado)
    : remitente(remitente),destinatario(destinatario),estado(estado)

{}

QString SolicitudContacto::getRemitente()const
{

    return remitente;

}

QString SolicitudContacto::getDestinaario()const
{

    return destinatario;

}

QString SolicitudContacto::getEstado()const
{

    return estado;

}

void SolicitudContacto::setEstado(const QString &NuevoEstado)
{

    estado=NuevoEstado;

}

QString SolicitudContacto::Serializar()const
{

    return remitente+ "|"+destinatario+"|"+estado;

}
