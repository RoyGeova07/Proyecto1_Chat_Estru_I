#ifndef SOLICITUDCONTACTO_H
#define SOLICITUDCONTACTO_H

#include<QString>

//tenog que crear un archivo para las solicitudees, para guardar las solicitudes de amistad
//estas solicitudes pueden estar: pendientes, aceptadas, rechazadas

//Guarda quien envio, a quien se envio y el estado (pendiente, aceptada, rechazada).
//Se guarda como linea de texto en solicitudes.txt.

class SolicitudContacto
{
public:
    SolicitudContacto(QString remitente="",QString destinatario="",QString estado="pendiente");

    QString getRemitente()const;
    QString getDestinaario()const;
    QString getEstado()const;

    void setEstado(const QString &NuevoEstado);

    QString Serializar()const;//este me servira para guardar en archivo


private:

    QString remitente;
    QString destinatario;
    QString estado;//pendientes, aceptadas, rechazadas

};

#endif // SOLICITUDCONTACTO_H
