#ifndef NOTIFICACION_H
#define NOTIFICACION_H

#include<QString>


class Notificacion
{
public:
    Notificacion(QString destino="",QString relacionado="",QString tipo="",QString mensaje="",QString estado="nueva");

    QString getDestino()const;
    QString getRelacionado()const;
    QString getTipo()const;
    QString getMensaje()const;
    QString getEstado()const;
    QString getFecha() const;
    QString getHora() const;

    void setEstado(const QString &nuevoEstado);

    QString serializar()const;
    static Notificacion deserializar(const QString &linea);

private:
    QString destino;
    QString relacionado;
    QString tipo;     // solicitud o respuesta
    QString mensaje;
    QString estado;   // nueva o vista
    QString fecha;
    QString hora;

};

#endif // NOTIFICACION_H
