#include "gestioncontactos.h"
#include<QFile>
#include<QTextStream>
#include<QDir>
#include"funciones_usuario.h"


//Como se carga la informacion (usuarios, contactos y solicitudes)?
// en cargar usuarios, se asume que carga todos los usuarios desde un archivo txt
//lee los archivos y convierte cada linea en nodos enlazados
//usa listas enlazadas (NodoSolicitud, NodoContacto) para recorrerlos dinamicamente.)

GestionContactos::GestionContactos()
{

    solicitudes=nullptr;
    contactos=nullptr;
    CargarDatos();

}

GestionContactos::~GestionContactos()
{

    liberarContactos();
    liberarSolicitudes();

}

bool GestionContactos::EnviarSolicitud(QString remitente, QString destinatario)
{

    //no se permite eviarse solicitud a uno mismo
    if(remitente==destinatario)return false;



    //aqui se revisa si ya existe  solicitud o ya son contactos
    NodoSolicitud*ActualSolicitud=solicitudes;
    while(ActualSolicitud)
    {

        if((ActualSolicitud->dato.getRemitente()==remitente&&ActualSolicitud->dato.getDestinaario()==destinatario)||(ActualSolicitud->dato.getRemitente()==destinatario&&ActualSolicitud->dato.getDestinaario()==remitente))
        {

            // si el estado es PENDIENTE o ACEPTADA, no se puede reenviar
            if(ActualSolicitud->dato.getEstado()=="pendiente"||ActualSolicitud->dato.getEstado()=="aceptada")
            {

                return false;

            }

            //si ya fue rechazada, se permite crear a una nueva solicitud
        }
        ActualSolicitud=ActualSolicitud->siguiente;

    }

    //aqui se verifican si ya son amigos
    NodoContacto*ActualContacto=contactos;
    while (ActualContacto)
    {

        if((ActualContacto->usuario1==remitente&&ActualContacto->usuario2==destinatario)||(ActualContacto->usuario1==destinatario&&ActualContacto->usuario2==remitente))
        {

            return false;

        }
        ActualContacto=ActualContacto->siguiente;

    }

    //aqui se crea una nueva solicitud
    NodoSolicitud*nuevo=new NodoSolicitud{SolicitudContacto(remitente,destinatario,"pendiente"),nullptr};

    if(!solicitudes)
    {

        solicitudes=nuevo;

    }else{

        NodoSolicitud* aux=solicitudes;
        while (aux->siguiente)
        {

            aux=aux->siguiente;

        }
        aux->siguiente=nuevo;

    }

    GuardarSolicitudes();
    return true;

}

bool GestionContactos::AceptarSolicitud(QString remitente, QString destinatario)
{

    NodoSolicitud*actual=solicitudes;
    while (actual)
    {

        if(actual->dato.getRemitente()==remitente&&actual->dato.getDestinaario()==destinatario&&actual->dato.getEstado()=="pendiente")
        {

            actual->dato.setEstado("aceptada");

            //aqui se crea nueva relacion de contacto
            NodoContacto*nuevo=new NodoContacto{remitente,destinatario,nullptr};

            if(!contactos)
            {

                contactos=nuevo;

            }else{

                NodoContacto*aux=contactos;
                while(aux->siguiente)
                {

                    aux=aux->siguiente;

                }
                aux->siguiente=nuevo;

            }
            GuardarSolicitudes();;
            GuardarContactos();
            return true;

        }
        actual=actual->siguiente;

    }
    return false;

}

bool GestionContactos::RechazarSolicitud(QString remitente, QString destinatario)
{

    NodoSolicitud*actual=solicitudes;
    while(actual)
    {

        if(actual->dato.getRemitente()==remitente&&actual->dato.getDestinaario()==destinatario&&actual->dato.getEstado()=="pendiente")
        {

            actual->dato.setEstado("rechazada");
            GuardarSolicitudes();
            return true;

        }
        actual=actual->siguiente;

    }
    return false;

}

bool GestionContactos::EliminarContacto(QString usuario1, QString usuario2)
{

    NodoContacto*actual=contactos;
    NodoContacto*anterior=nullptr;

    while(actual)
    {

        if((actual->usuario1==usuario1&&actual->usuario2==usuario2)||(actual->usuario1==usuario2&&actual->usuario2==usuario1))
        {

            if(anterior==nullptr)
            {

                contactos=actual->siguiente;

            }else{

                anterior->siguiente=actual->siguiente;

            }
            delete actual;
            GuardarContactos();
            return true;

        }
        anterior=actual;
        actual=actual->siguiente;

    }
    return false;

}

QStringList GestionContactos::ObtenerContactos(QString usuario) const
{

    QStringList lista;
    NodoContacto*actual=contactos;

    while(actual)
    {

        if(actual->usuario1==usuario)
        {

            lista.append(actual->usuario2);

        }else if(actual->usuario2==usuario){

            lista.append(actual->usuario1);

        }
        actual=actual->siguiente;


    }
    return lista;

}

QStringList GestionContactos::ObtenerSolicitudesPendientes(QString usuario) const
{

    QStringList lista;
    NodoSolicitud*actual=solicitudes;

    while(actual)
    {

        if(actual->dato.getDestinaario()==usuario&&actual->dato.getEstado()=="pendiente")
        {

            lista.append(actual->dato.getRemitente());

        }
        actual=actual->siguiente;

    }

    return lista;

}

QString GestionContactos::ObtenerEstadoSolicitud(QString remitente, QString destinatario) const
{
    QString EstadoMasReciente="";
    NodoSolicitud*actual=solicitudes;
    while(actual)
    {

        if(actual->dato.getRemitente()==remitente&&actual->dato.getDestinaario()==destinatario)
        {

            EstadoMasReciente=actual->dato.getEstado();//aqui guarda la mas reciente

        }
        actual=actual->siguiente;

    }
    return EstadoMasReciente;

}



void GestionContactos::CargarDatos()
{

    liberarSolicitudes();
    liberarContactos();

    //aqui se cargan las solicitudessszzz
    QFile ArchivoSoli(RutaSolicitudes());
    if(ArchivoSoli.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&ArchivoSoli);
        while(!in.atEnd())
        {
            //el split lo uso para  dividir una cadena en multiples partes
            QString linea=in.readLine();
            QStringList partes=linea.split("|");
            if(partes.size()==3)
            {

                NodoSolicitud*nuevo=new NodoSolicitud{SolicitudContacto(partes[0],partes[1],partes[2]),nullptr};

                if(!solicitudes)
                {

                    solicitudes=nuevo;

                }else{

                    NodoSolicitud*aux=solicitudes;
                    while(aux->siguiente)
                    {

                        aux=aux->siguiente;

                    }
                    aux->siguiente=nuevo;

                }

            }


        }
        ArchivoSoli.close();

    }

    //aqui se cargan los contactos
    QFile  ArchivoContac(RutaContactos());
    if(ArchivoContac.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&ArchivoContac);
        while(!in.atEnd())
        {

            QString linea=in.readLine();
            QStringList partes=linea.split("|");
            if(partes.size()==2)
            {

                NodoContacto*nuevo=new NodoContacto{partes[0],partes[1],nullptr};

                if(!contactos)
                {

                    contactos=nuevo;

                }else{

                    NodoContacto*aux=contactos;
                    while(aux->siguiente)
                    {

                        aux=aux->siguiente;

                    }
                    aux->siguiente=nuevo;

                }

            }

        }
        ArchivoContac.close();

    }

}

//aqui se guardan las solicitudes en el archivo
void GestionContactos::GuardarSolicitudes()const
{

    //escribimos en el archivo
    QFile Archivo(RutaSolicitudes());
    if(Archivo.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        QTextStream out(&Archivo);
        NodoSolicitud*actual=solicitudes;
        while (actual)
        {

            out<<actual->dato.Serializar()<<"\n";
            actual=actual->siguiente;

        }
        Archivo.close();

    }

}

//aqui se guardan los contactos en archivo
void GestionContactos::GuardarContactos()const
{

    QFile archivo(RutaContactos());
    if(archivo.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        QTextStream out(&archivo);
        NodoContacto*actual=contactos;
        while(actual)
        {

            out<<actual->usuario1<<"|"<<actual->usuario2<<"\n";
            actual=actual->siguiente;

        }
        archivo.close();

    }

}

void GestionContactos::liberarSolicitudes()
{

    NodoSolicitud*actual=solicitudes;
    while(actual)
    {

        NodoSolicitud*aux=actual;
        actual=actual->siguiente;
        delete aux;

    }
    solicitudes=nullptr;

}

bool GestionContactos::TieneSolicitudRecibida(const QString &yo, const QString &otro)
{

    NodoSolicitud*actual=solicitudes;
    while(actual)
    {

        if(actual->dato.getRemitente()==otro&&actual->dato.getDestinaario()==yo&&actual->dato.getEstado()=="pendiente")
        {

            return true;

        }
        actual=actual->siguiente;

    }
    return false;

}

bool GestionContactos::ExisteContacto(const QString& usuario1, const QString& usuario2) const {
    NodoContacto* actual = contactos;
    while (actual) {
        if ((actual->usuario1 == usuario1 && actual->usuario2 == usuario2) ||
            (actual->usuario1 == usuario2 && actual->usuario2 == usuario1)) {
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

void GestionContactos::liberarContactos()
{

    NodoContacto*actual=contactos;
    while(actual)
    {

        NodoContacto*aux=actual;
        actual=actual->siguiente;
        delete aux;

    }
    contactos=nullptr;

}
