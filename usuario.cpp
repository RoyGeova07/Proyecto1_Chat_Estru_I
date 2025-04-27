#include "usuario.h"

Usuario::Usuario(QString usuario,QString nombre,QString correo,QString contrasena,int edad,QString pregunta,QString respuesta,QString rutaAvatar,bool conectado)
    :usuario(usuario),nombre(nombre),correo(correo),contrasena(contrasena),edad(edad),pregunta(pregunta),respuesta(respuesta),
    rutaAvatar(rutaAvatar),conectado(conectado)
{}

//aqui gettersssss
QString Usuario::getUsuario()const {return usuario;}
QString Usuario::getNombre()const{return nombre;}
QString Usuario::getCorreo()const{return correo;}
QString Usuario::getContrasena()const{return contrasena;}
int Usuario::getEdad()const{return edad;}
QString Usuario::getPregunta()const{return pregunta;}
QString Usuario::getRespuesta()const{return respuesta;}
QString Usuario::getRutaAvatar()const{return rutaAvatar;}
bool Usuario::estaConectado()const{return conectado;}

//setterssss
void Usuario::setEstado(bool conectado)
{

    this->conectado=conectado;

}

//serializarrrrrr
QString Usuario::serializar()const
{

    return usuario+"|"+nombre+"|"+correo+"|"+contrasena+"|"+QString::number(edad)+"|"+pregunta+"|"+respuesta+"|"+
           rutaAvatar+"|"+(conectado?"1":"0");

}
