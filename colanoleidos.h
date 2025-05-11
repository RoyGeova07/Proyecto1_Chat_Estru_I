#ifndef COLANOLEIDOS_H
#define COLANOLEIDOS_H

#pragma once
#include"Mensaje.h"

template<typename T>
struct NodoCola
{

    T dato;
    NodoCola*siguiente;
    NodoCola(const T& d) : dato(d),siguiente(nullptr) {}

};


template<typename T>
class ColaNoLeidos
{
public:
    ColaNoLeidos();
    ~ColaNoLeidos();

    //aqui inserta en cola
    void Insertar(const T& d);
    T Quitar();
    T frente()const;
    bool empty()const;
    int size()const;

private:
    NodoCola<T> *cabeza;
    NodoCola<T> *cola;
    int count;


};

#endif // COLANOLEIDOS_H
