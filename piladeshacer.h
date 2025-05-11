#ifndef PILADESHACER_H
#define PILADESHACER_H

#pragma once
#include"Mensaje.h"


template<typename T>
struct NodoPila
{

    T dato;
    NodoPila*siguiente;
    NodoPila(const T& d,NodoPila*sig=nullptr):dato(d),siguiente(sig){}

};

//pila para deshacer el ultimo mensaje

template<typename T>
class PilaDeshacer
{
private:

    NodoPila<T>*tope;


public:
    PilaDeshacer():tope(nullptr){}

    ~PilaDeshacer()
    {

        while(tope)
        {

            NodoPila<T>*sig=tope->siguiente;
            delete tope;
            tope=sig;

        }

    }


    //aqui se mete un mensaje en la pila
    void insertar(const T& d)
    {

        tope=new NodoPila<T>(d,tope);

    }
    //saca y devuelve el mensaje tope
    T pop()
    {

        if(!tope) throw std::runtime_error("Pila vacia bro");
        T d=tope->dato;
        NodoPila<T>*viejo=tope;
        tope=tope->siguiente;
        delete viejo;
        return d;

    }
    // Devuelve el tope sin quitarlo
    T top()const
    {

        if(!tope)throw std::runtime_error("Pila vacia bro");
        return tope->dato;

    }
    bool empty()const
    {

        return tope==nullptr;

    }

};

#endif // PILADESHACER_H
