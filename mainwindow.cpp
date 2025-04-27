#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QLabel>
#include<QPushButton>
#include<QVBoxLayout>
#include<QPixmap>

MainWindow::MainWindow(Usuario usuario,QWidget *parent)
    : QMainWindow(parent)
    , UsuarioActivo(usuario)
{

    //aqui creo los widgets
    QWidget *central=new QWidget(this);
    QLabel *lblSaludo=new QLabel("Bienvenido,"+UsuarioActivo.getNombre()+"!",this);
    QLabel *lblEstado=new QLabel("Conectado",this);
    QLabel *lblAvatar=new QLabel(this);
    QPushButton *btnChat=new QPushButton("Chat",this);
    QPushButton *btnContactos=new QPushButton("Contactos",this);
    QPushButton *btnCerrar=new QPushButton("Cerrar sesión",this);

    //aqui para cargar avtar
    QPixmap avatar(UsuarioActivo.getRutaAvatar());
    lblAvatar->setPixmap(avatar.scaled(100,100,Qt::KeepAspectRatio));
    lblAvatar->setFixedSize(100,100);

    //layouts
    QVBoxLayout *layoutPrincipal=new QVBoxLayout();
    layoutPrincipal->addWidget(lblAvatar,0,Qt::AlignCenter);
    layoutPrincipal->addWidget(lblSaludo,0,Qt::AlignCenter);
    layoutPrincipal->addWidget(lblEstado,0,Qt::AlignCenter);
    layoutPrincipal->addWidget(btnChat);
    layoutPrincipal->addWidget(btnContactos);
    layoutPrincipal->addWidget(btnCerrar);

    //aqui se asigna el layout al widget central
    central->setLayout(layoutPrincipal);
    setCentralWidget(central);

    //tamanio y titulo de la ventana
    resize(400,500);
    setWindowTitle("Chat App - Usuario");

    //conectar sellales boton cerrarrrr
    connect(btnCerrar,&QPushButton::clicked,this,&MainWindow::close);


}

