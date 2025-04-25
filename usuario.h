#ifndef USUARIO_H
#define USUARIO_H

#include<QString>

class Usuario
{
public:

    Usuario(QString usuario="",QString nombre="",QString correo="",QString contrasena="",int edad=0,QString pregunta="",
    QString respuesta="",QString rutaAvatar="",bool conectado=false);

    //aqui getters
    QString getUsuario()const;
    QString getNombre()const;
    QString getCorreo()const;
    QString getContrasena()const;
    int getEdad()const;
    QString getPregunta()const;
    QString getRespuesta()const;
    QString getRutaAvatar()const;
    bool estaConectado()const;

    void setEstado(bool conectado);

    QString serializar()const;

private:

    QString usuario;
    QString nombre;
    QString correo;
    QString contrasena;
    int edad;
    QString pregunta;
    QString respuesta;
    QString rutaAvatar;
    bool conectado;//true=conectado bro,


};

#endif // USUARIO_H
