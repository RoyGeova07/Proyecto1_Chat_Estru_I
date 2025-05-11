#include "colanoleidos.h"
#include<stdexcept>


template<typename T>
ColaNoLeidos<T>::ColaNoLeidos()
    : cabeza(nullptr), cola(nullptr), count(0)
{}


template<typename T>
ColaNoLeidos<T>::~ColaNoLeidos()
{

    //aqui se libera toda la cola
    while(cabeza)
    {

        NodoCola<T> *sig=cabeza->siguiente;
        delete cabeza;
        cabeza=sig;

    }

}

template<typename T>
void ColaNoLeidos<T>::Insertar(const T& d)
{

    NodoCola<T>*nodo=new NodoCola<T>(d);
    if(!cola)
    {

        //cola vacia
        cabeza=cola=nodo;

    }else{

        cola->siguiente=nodo;
        cola=nodo;

    }
    ++count;

}

template<typename T>
T ColaNoLeidos<T>::Quitar()
{

    if(!cola)throw std::runtime_error("Cola vacia crack");
    T d =cabeza->dato;
    NodoCola<T>viejo=cabeza;
    cabeza=cabeza->siguiente;
    if(!cabeza)cola=nullptr;
    delete viejo;
    --count;
    return d;

}


template<typename T>
T ColaNoLeidos<T>::frente()const
{

    if(!cola) throw std::runtime_error("cola vaciaaaaa");
    return cabeza->dato;

}


template<typename T>
bool ColaNoLeidos<T>::empty()const
{

    return cabeza==nullptr;

}


template<typename T>
int ColaNoLeidos<T>::size()const
{

    return count;

}
