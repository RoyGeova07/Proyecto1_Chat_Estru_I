#ifndef GESTIONCONTACTOS_H
#define GESTIONCONTACTOS_H

#include"solicitudcontacto.h"
#include<QList>

//Carga solicitudes y contactos al iniciar

//Permite enviar, aceptar, rechazar solicitudes

//Actualiza solicitudes.txt y contactos.txt

//Puede devolver la lista de contactos de un usuario o sus solicitudes pendientes.

//Estructura que guarda un SolicitudContacto y un puntero al siguiente nodo.
struct NodoSolicitud
{

    SolicitudContacto dato;
    NodoSolicitud*siguiente;

};

//Estructura que guarda dos usuarios y un puntero al siguiente nodo.
struct NodoContacto
{

    QString usuario1;
    QString usuario2;
    NodoContacto*siguiente;

};

//solicitudes	Apunta al primer NodoSolicitud (lista enlazada).

//contactos	Apunta al primer NodoContacto (lista enlazada).

//liberarSolicitudes()	Funcion para liberar toda la memoria de la lista cuando destruyamos el objeto.

//liberarContactos()	Igual pero para los contactos.



//Ahora cada vez que agreguemos una solicitud o un contacto:

//Hay que hacer new NodoSolicitud o new NodoContacto.

//    Hay que insertarlo manualmente en la lista (normalmente al final).

//    Cuando eliminemos, hay que ajustar manualmente los punteros (anterior->siguiente = actual->siguiente).

//    Tambien:

//   Al cerrar el programa o destruir la clase, hay que recorrer toda la lista liberando memoria (delete).


class GestionContactos
{
public:
    GestionContactos();
    ~GestionContactos();//liberar memoriaaaaaa

    bool EnviarSolicitud(QString remitente,QString destinatario);
    bool AceptarSolicitud(QString remitente,QString destinatario);
    bool RechazarSolicitud(QString remitente,QString destinatario);
    bool EliminarContacto(QString usuario1,QString usuario2);

    QStringList ObtenerContactos(QString usuario)const;
    QStringList ObtenerSolicitudesPendientes(QString usuario)const;


private:

    NodoSolicitud*solicitudes;// cabeza de la lista de solitudes
    NodoContacto*contactos;// cabeza de la lista de contactos

    void CargarDatos();
    void GuardarSolicitudes()const;
    void GuardarContactos()const;


    void liberarSolicitudes(); // para liberar memoria
    void liberarContactos();


};

#endif // GESTIONCONTACTOS_H
