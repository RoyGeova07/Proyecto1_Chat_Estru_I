#ifndef MENSAJE_H
#define MENSAJE_H

#pragma once
#include <QString>
#include <QDateTime>

template<typename T>
class Mensaje {
public:

    enum Tipo{TEXTO,STICKER,ARCHIVO};


    Mensaje(){}
    Mensaje(const QString &emisor, const QString &receptor, const T &contenido, Tipo tipo, const QDateTime &fecha)
        : emisor(emisor), receptor(receptor), contenido(contenido), tipo(tipo), fecha(fecha) {}

    QString getEmisor()const{return emisor;}
    QString getReceptor()const{return receptor;}
    T getContenido()const{return contenido;}
    Tipo getTipo()const {return tipo;}
    QDateTime getFecha()const{return fecha;}

    void setContenido(const T &nuevo){contenido=nuevo;}

    QString tipoComoTexto() const {
        switch (tipo) {
        case TEXTO: return "texto";
        case STICKER: return "sticker";
        case ARCHIVO: return "archivo";
        }
        return "desconocido";
    }


private:

    QString emisor;
    QString receptor;
    T contenido;
    Tipo tipo;
    QDateTime fecha;

};

#endif // MENSAJE_H
