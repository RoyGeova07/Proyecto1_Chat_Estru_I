#include "inicio.h"
#include<QPushButton>
#include<QLabel>
#include<QVBoxLayout>
#include<QPixmap>
#include<QPalette>
#include"registro.h"
#include"login.h"

Inicio::Inicio(QWidget *parent)
    : QWidget(parent)

{

    this->resize(800,600);//tamanioooo

    //para el fondo de imagen

    QPixmap background("C:/Users/royum/OneDrive/Documentos/Proyecto1_Chat_Estru_I/Imagenes/Fondo.jpg");
    background=background.scaled(this->size(),Qt::IgnoreAspectRatio);

    //el setBrush establece el pincel o el estilo de relleno de un objeto grafico
    //este pincel determina como se rellena un área o forma, ya sea con un color solido, una textura o un patron

    QPalette palette;
    palette.setBrush(QPalette::Window,background);
    this->setPalette(palette);

    //tituloooo
    lblTitulo=new QLabel("Proyecto Chat - Estru I",this);
    //si preguntan, esto es sintaxis estilo css para hacer paginas web jejej
    lblTitulo->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
    lblTitulo->setAlignment(Qt::AlignCenter);

    //botonessss
    btnLogin=new QPushButton("Iniciar Sesion",this);
    btnCrearCuenta=new QPushButton("Crear Cuenta",this);
    btnSalir=new QPushButton("Salir",this);

    //estilo de botones
    QString EstiloBotones= "QPushButton {"
        "background-color: white;"
        "color: black;"
        "font-size: 18px;"
        "border-radius: 10px;"
        "padding: 10px"
        "}"
        "QPushButton:hover {"
        "background-color: lightblue;"
        "}";

    btnLogin->setStyleSheet(EstiloBotones);
    btnCrearCuenta->setStyleSheet(EstiloBotones);
    btnSalir->setStyleSheet(EstiloBotones);

    //layoutssss
    QVBoxLayout *layout=new QVBoxLayout(this);
    layout->addWidget(lblTitulo);
    layout->addSpacing(100);//aqui espacio entre botones
    layout->addWidget(btnLogin);
    layout->addWidget(btnCrearCuenta);
    layout->addWidget(btnSalir);

    layout->setAlignment(Qt::AlignCenter);
    setLayout(layout);

    setWindowTitle("Proyecto Chat - Estru I");

    //conexionesss
    connect(btnLogin,&QPushButton::clicked,this,&Inicio::AbrirLogIn);
    connect(btnCrearCuenta,&QPushButton::clicked,this,&Inicio::AbrirRegistro);
    connect(btnSalir,&QPushButton::clicked,this,&QWidget::close);


}

void Inicio::AbrirLogIn()
{

    LogIn *l=new LogIn();
    l->show();
    this->close();

}

void Inicio::AbrirRegistro()
{

    Registro *r=new Registro();
    r->show();
    this->close();

}
