#include "login.h"
#include "ui_login.h"
#include"mainwindow.h"
#include<QMessageBox>
#include"funciones_usuario.h"
#include"registro.h"
#include"inicio.h"

LogIn::LogIn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogIn)
{
    ui->setupUi(this);

    connect(ui->btnLogin,&QPushButton::clicked,this,&LogIn::VerificarCredenciales);
    connect(ui->crear,&QPushButton::clicked,this,&LogIn::AbrirRegistro);
    connect(ui->btnRegresar,&QPushButton::clicked,this,&LogIn::AbrirInicio);

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

            QMessageBox::information(this,"Exito",u.getUsuario()+" A iniciado sesion con exito!!");

            MainWindow *main=new MainWindow(u);
            main->show();
            this->close();

            return;

        }

    }
    QMessageBox::warning(this,"Error","Usuario o contraseña incorrectos");

}
