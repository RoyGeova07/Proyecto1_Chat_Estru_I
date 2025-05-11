#include "login.h"
#include "ui_login.h"
#include"mainwindow.h"
#include<QMessageBox>
#include"funciones_usuario.h"
#include"registro.h"
#include"inicio.h"
#include<QList>
#include<QInputDialog>
#include"mainwindow.h"

LogIn::LogIn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogIn)
{
    ui->setupUi(this);

    connect(ui->btnLogin,&QPushButton::clicked,this,&LogIn::VerificarCredenciales);
    connect(ui->crear,&QPushButton::clicked,this,&LogIn::AbrirRegistro);
    connect(ui->btnRegresar,&QPushButton::clicked,this,&LogIn::AbrirInicio);
    connect(ui->recuperar,&QPushButton::clicked,this,&LogIn::RecuperarCuenta);

}

LogIn::~LogIn()
{
    delete ui;
}


void LogIn::AbrirRegistro()
{

    Registro *registro=new Registro();
    registro->show();
    this->close();

}

void LogIn::AbrirInicio()
{

    Inicio *i=new Inicio();
    i->show();
    this->close();

}

void LogIn::VerificarCredenciales()
{

    QString user=ui->txtUsuario->text();
    QString pass=ui->txtContrasena->text();

    QList<Usuario> usuarios=CargarUsuarios();

    for(const Usuario &u:usuarios)
    {

        if(u.getUsuario()==user&&u.getContrasena()==pass)
        {

            QMessageBox::information(this,"Exito",u.getUsuario()+" A iniciado sesion y activado su cuenta con exito!!");
            MarcarUsuarioComoConectado(u.getUsuario());

            MainWindow *main=new MainWindow(u);
            main->show();
            this->close();

            return;

        }

    }
    QMessageBox::warning(this,"Error","Usuario o contraseña incorrectos");

}

void LogIn::RecuperarCuenta()
{

    QList<Usuario> usuarios=CargarUsuarios();

    //aqui se hace el primer input, se pide el nombre del usuariooooooo
    bool ok;
    QString nombre=QInputDialog::getText(this,"Recuperar cuenta","Ingrese nombre de usuario a recuperar:",QLineEdit::Normal,"",&ok);

    if(!ok||nombre.isEmpty())
    {

        QMessageBox::warning(this,"Error","Respuesta vacia");
        return;

    }

    //aqui se busca el usuario
    for(const Usuario &coca:usuarios)
    {

        if(coca.getUsuario()==nombre)
        {

            //aqui el segundo input: se muestra la pregunta acorde como lo guardo el usuario
            QString pregunta=coca.getPregunta();
            //se pasa por referencia el bool porque  Qt necesita modificar su valor dentro de la funcion y devolverlo.
            //si se le pasara por valor (como bool ok), Qt no podria cambiarlo.
            //Al usar bool &ok, lo que cambia dentro de getText() se refleja directamente fuera, como una salida adicional.

            //si el usuario presiona "Aceptar", ok = true.

            //si el usuario presiona "Cancelar", ok = false.

            QString respuesta=QInputDialog::getText(this,"Pregunta de seguridad",pregunta,QLineEdit::Normal,"",&ok);

            if(!ok||respuesta.isEmpty())
            {

                QMessageBox::warning(this,"Error","Respuesta vacia");
                return;

            }

            if(respuesta==coca.getRespuesta())
            {

                QMessageBox::information(this,"Acceso concedido","Bienvenido "+coca.getUsuario());
                MainWindow *entrar=new MainWindow(coca);
                entrar->show();
                this->close();

                QString nuevacontra;
                bool cambiobro;

                nuevacontra=QInputDialog::getText(this,"Cambio de contraseña."," QT detecto que entraste por tu pregunta de seguridad para evitar de nuevo este problema cambie su contraseña si lo desea\nIngresa la nueva contraseña:",QLineEdit::Password,"",&cambiobro);

                if(cambiobro&&!nuevacontra.isEmpty())
                {

                    QList<Usuario> usuarios=CargarUsuarios();

                    for(Usuario &u:usuarios)
                    {

                        if(u.getUsuario()==coca.getUsuario())
                        {

                            u.setContrasena(nuevacontra);
                            break;

                        }

                    }
                    GuardarUsuarios(usuarios);
                    QMessageBox::information(this, "Cambio exitoso", "¡Contraseña actualizada correctamente!");

                }

            }else{

                QMessageBox::warning(this,"Error","Respuesta Incorrecta");

            }
            return;

        }

    }
    QMessageBox::warning(this,"Error","Usuario no encontrado bro, no mienta");

}
