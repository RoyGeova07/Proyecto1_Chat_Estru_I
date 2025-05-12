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

private:
    NodoCola<T> *cabeza;
    NodoCola<T> *cola;
    int count;


public:
    ColaNoLeidos():cabeza(nullptr),cola(nullptr),count(0){}
    ~ColaNoLeidos()
    {

        while(cabeza)
        {

            NodoCola<T>*siguiente=cabeza->siguiente;
            delete cabeza;
            cabeza=siguiente;

        }

    }
    //aqui inserta en cola
    void Insertar(const T& d)
    {

        NodoCola<T>*nodo=new NodoCola<T>(d);
        if(!cola)
        {

            cabeza=cola=nodo;

        }else{

            cola->siguiente=nodo;
            cola=nodo;

        }
        ++count;

    }

    T Quitar()
    {

        if(!cabeza)throw std::runtime_error("Cola vacia");
        T d=cabeza->dato;
        NodoCola<T>*viejo=cabeza;
        cabeza=cabeza->siguiente;
        if(!cabeza)cola=nullptr;
        delete viejo;
        --count;
        return d;

    }
    T frente()const
    {

        if(!cabeza)throw std::runtime_error("cola vacia crack");
        return cabeza->dato;

    }
    bool empty()const
    {

        return cabeza==nullptr;

    }
    int size()const
    {

        return count;

    }



};

#endif // COLANOLEIDOS_H
