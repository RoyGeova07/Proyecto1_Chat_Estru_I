#include"mainwindow.h"
#include"inicio.h"
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QLabel>
#include<QPushButton>
#include<QPixmap>
#include<QListWidget>
#include<QTextEdit>
#include<QStackedWidget>
#include<QInputDialog>
#include<QMessageBox>
#include<QScrollArea>
#include<QIcon>
#include<QSpacerItem>
#include<QLineEdit>
#include<QDir>
#include<QFile>
#include<QTextStream>
#include"funciones_usuario.h"
#include"funciones_conectados.h"

MainWindow::MainWindow(Usuario usuario,QWidget *parent)
    : QMainWindow(parent)
    , UsuarioActivo(usuario)

{

    QWidget *central=new QWidget(this);
    setCentralWidget(central);
    resize(1100,650);
    setWindowTitle("Proyecto Chat - Usuario");

    //aqui sale el fondoooo
    central->setStyleSheet("background-color: #f5f6fa;");

    // ========aqui se crea la barra lateral============
    QWidget*barraLateral=new QWidget(this);
    barraLateral->setFixedWidth(220);
    barraLateral->setStyleSheet("background-color: #273c75");

    QVBoxLayout*layoutLateral=new QVBoxLayout(barraLateral);

    QLabel*lblAvatar=new QLabel(this);
    QPixmap avatar(UsuarioActivo.getRutaAvatar());//obtenemos la ruta del avatar
    lblAvatar->setPixmap(avatar.scaled(100,100,Qt::KeepAspectRatio,Qt::SmoothTransformation));//aqui se modifica su tamanio
    lblAvatar->setAlignment(Qt::AlignCenter);//agregarlo en lugar centrado el avatar

    QLabel*lblNombre=new QLabel(UsuarioActivo.getNombre(),this);
    lblNombre->setAlignment(Qt::AlignCenter);
    lblNombre->setStyleSheet("color: white; font-weight: bold; font-size: 18px");

    //botones para barra lateral
    btnBuscar=new QPushButton(QIcon(":/icons/search.png")," Buscar",this);
    btnMensajes=new QPushButton(QIcon(":/icons/messages.png")," Mensajes",this);
    btnContactos=new QPushButton(QIcon(":/icons/contactos.png")," Contactos",this);
    btnCerrarSesion=new QPushButton(QIcon(":/icons/cerrar.png")," Cerrar Sesion",this);

    QString EstiloBoton="QPushButton {background-color: transparent; color: white;  font-size: 16px; padding: 12px; text-align: left;}"
                            "QPushButton:hover {background-color: #40739e; }";

    btnBuscar->setStyleSheet(EstiloBoton);
    btnContactos->setStyleSheet(EstiloBoton);
    btnMensajes->setStyleSheet(EstiloBoton);
    btnCerrarSesion->setStyleSheet(EstiloBoton);



    //Aqui se crean los paneles derechooo
    layoutLateral->addWidget(lblAvatar);
    layoutLateral->addWidget(lblNombre);
    layoutLateral->addSpacing(220);
    layoutLateral->addWidget(btnBuscar);
    layoutLateral->addWidget(btnContactos);
    layoutLateral->addWidget(btnMensajes);
    layoutLateral->addStretch();
    layoutLateral->addWidget(btnCerrarSesion);


    // ======= aqui se crea el panel derecho ======
    paneles=new QStackedWidget(this);
    paneles->addWidget(crearPanelBuscar());//aqui con el indiice 0
    paneles->addWidget(crearPanelContactos());//aqui con el indice 1
    paneles->addWidget(crearPanelMensajes());//aqui con el indice 2

    //aqui se crea el layout principal
    QHBoxLayout *layoutPrincipal=new QHBoxLayout(central);
    layoutPrincipal->addWidget(barraLateral);
    layoutPrincipal->addWidget(paneles);

    // =======CONEXIONES======
    connect(btnBuscar,&QPushButton::clicked,this,&MainWindow::mostrarPanelBuscar);
    connect(btnContactos,&QPushButton::clicked,this,&MainWindow::mostrarPanelContactos);
    connect(btnMensajes,&QPushButton::clicked,this,&MainWindow::mostrarPanelMensajes);
    connect(btnCerrarSesion,&QPushButton::clicked,this,&MainWindow::cerrarSesion);


    //aqui se mostrara el panel por defecto
    paneles->setCurrentIndex(0);
}

// =====AQUI FUNCIONES PARA CAMBIAR DE PANEL=====
void MainWindow::mostrarPanelBuscar()
{

    paneles->setCurrentIndex(0);

}

void MainWindow::mostrarPanelContactos()
{

    paneles->setCurrentIndex(1);

}

void MainWindow::mostrarPanelMensajes()
{

    paneles->setCurrentIndex(2);

}

void MainWindow::cerrarSesion()
{

    Inicio *i=new Inicio();
    i->show();
    this->close();

}

// ====AQUI FUNCIONES PARA CREAR LOS PANELES=====
QWidget *MainWindow::crearPanelBuscar()
{
    QWidget *panel = new QWidget;
    panel->setStyleSheet("background-color: #ecf0f1;");
    QVBoxLayout *layoutPanel = new QVBoxLayout(panel);
    layoutPanel->setContentsMargins(20, 20, 20, 20);
    layoutPanel->setSpacing(10);

    //aqui campo de busqueda
    QLineEdit *buscador=new QLineEdit;
    buscador->setPlaceholderText("Buscar usuario...");
    buscador->setStyleSheet("padding: 10px; font-size: 14px; color: black; background-color: white; border: 1px solid #ccc; border-radius: 5px;");
    layoutPanel->addWidget(buscador);

    //aqui area de scroll
    QScrollArea *scroll=new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget *contenedorUsuarios=new QWidget;
    QVBoxLayout *layoutUsuarios=new QVBoxLayout(contenedorUsuarios);
    layoutUsuarios->setSpacing(15);
    contenedorUsuarios->setStyleSheet("background-color: transparent;");

    //como funcionara el filtro de busqueda?

    //se guardara cada targeta card de usuario en una lista para poder manipularla despues facilmente

    //se conecta el campo QLineEdit a una funcion lambda que se ejecuta cada vez que cambia el texto

    //busca dentro de cada card un QLABEL con el nombre de  "lblNombreUsuario"
    //elimina la parte  "Nombre de usuario: " para quedarse solo con el valor real
    //compara si ese nombre contiene el texto ingresado
    //si si, muestra la tarjeta, Si no, la oculta

    QList<QWidget*> tarjetasUsuarios;

    QList<Usuario> usuarios=CargarUsuarios();
    //aqui ordenamiento burbuja, para ordenar la lista de contacto en orden alfabetico

    //se usa un doble for, el externo recorre todo el arreglo, el interno compara pares adyacentes de usuarios (usuarios[j] y usuarios[j+1]).

    //los convierte en minusculas para que la comparacion no sea sensible a mayusculas y minusculas

    //si el usuario actual va despues alfabaticamente que el siguiente nombre1 > nombre2 los intercambia

    for(int i=0;i<usuarios.size()-1;++i)
    {

        for(int j=0;j<usuarios.size()-i-1;++j)
        {

            QString nombre1=usuarios[j].getUsuario().toLower();
            QString nombre2=usuarios[j+1].getUsuario().toLower();
            if(nombre1>nombre2)
            {

                Usuario aux=usuarios[j];
                usuarios[j]=usuarios[j+1];
                usuarios[j+1]=aux;

            }

        }

    }

    for(const Usuario&u:usuarios) {
        if(u.getUsuario()==UsuarioActivo.getUsuario()) continue;

        // Tarjeta de usuario bro xd
        QWidget *card=new QWidget;
        card->setStyleSheet("background-color: white; border: 1px solid #dcdde1; border-radius: 10px;");
        QHBoxLayout *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(15, 10, 15, 10);
        cardLayout->setSpacing(15);

        // Avatar
        QLabel *avatar=new QLabel;
        QPixmap pix(u.getRutaAvatar());
        avatar->setPixmap(pix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        avatar->setFixedSize(60, 60);
        avatar->setStyleSheet("border-radius: 30px;");

        // Info del usuario
        QLabel *lblNombre=new QLabel("Nombre de usuario: " + u.getUsuario());
        lblNombre->setObjectName("lblNombreUsuario");
        lblNombre->setStyleSheet("font-size: 15px; font-weight: bold; color: #2f3640;");

        QString estadoTexto=EstaConectado(u.getUsuario())?"Online":"Offline";
        QString colorEstado=EstaConectado(u.getUsuario())?"green":"red";
        QLabel *estado=new QLabel("Estado: "+estadoTexto);
        estado->setStyleSheet("color: "+colorEstado+"; font-size: 13px;");

        QVBoxLayout *infoLayout=new QVBoxLayout;
        infoLayout->addWidget(lblNombre);
        infoLayout->addWidget(estado);
        infoLayout->setSpacing(4);

        //aqui el boton agregar
        QPushButton *btnSolicitud=new QPushButton("Agregar");
        btnSolicitud->setStyleSheet(

            "QPushButton { background-color: #3498db; color: white; padding: 8px 16px; font-size: 13px; border-radius: 6px; }"
            "QPushButton:disabled { background-color: gray; }"

            );

        if(gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario()).contains(u.getUsuario()))
        {

            btnSolicitud->setText("Amigos");
            btnSolicitud->setEnabled(false);

        }else if(gestionContactos.ObtenerSolicitudesPendientes(u.getUsuario()).contains(UsuarioActivo.getUsuario())){

            btnSolicitud->setText("Solicitud enviada");
            btnSolicitud->setEnabled(false);

        }

        //esta es una funcion lamba
        //el  [=] → captura por valor

        //aqui todas las variables que estan en el entorno (como UsuarioActivo, u, btnSolicitud) se copian dentro de la lambda.

        //aqui si no usa [=], Qt no sabria de donde sacar el u, ni UsuarioActivo, ni btnSolicitud.

        connect(btnSolicitud, &QPushButton::clicked,[=]()
        {

            if(gestionContactos.EnviarSolicitud(UsuarioActivo.getUsuario(), u.getUsuario()))
            {

                btnSolicitud->setText("Solicitud enviada");
                btnSolicitud->setEnabled(false);

            }

        });

        cardLayout->addWidget(avatar);
        cardLayout->addLayout(infoLayout);
        cardLayout->addStretch();
        cardLayout->addWidget(btnSolicitud);

        layoutUsuarios->addWidget(card);
        tarjetasUsuarios.append(card);
    }

    scroll->setWidget(contenedorUsuarios);
    layoutPanel->addWidget(scroll);

    // Filtro funcional
    //que hace este connect?
    //Filtra las tarjetas de usuarios mientras escribes texto en el buscador.

    //buscador: es el QLineEdit donde el usuario escribe texto.
    //&QLineEdit::textChanged: es la señal que se emite cada vez que cambia el texto del campo.

    //[=](const QString &texto) { ... }: es una lambda que se ejecuta al detectar cambios.
    //El parametro texto es el texto nuevo ingresado.
    connect(buscador,&QLineEdit::textChanged, [=](const QString &texto)
    {

        //tarjetasUsuarios
        //es una lista de todas las tarjetas de usuarios que se mostraron al cargar el panel.
        //El for recorre cada tarjeta (card) para decidir si debe mostrarse u ocultarse segun lo que el usuario escribio.
        for(QWidget *card:tarjetasUsuarios)
        {

            //el findChild<QLabel*>, Busca dentro de card un QLabel que tenga el nombre de objeto "lblNombreUsuario",
            //Es el QLabel que contiene algo como: "Nombre de usuario: juan123".
            QLabel *nombreLabel=card->findChild<QLabel *>("lblNombreUsuario");
            if (nombreLabel)
            {

                //aqui Limpiamos
                //el texto quitamos el prefijo "Nombre de usuario: " para quedarnos solo con el nombre real, por ejemplo "juan123".
                //luego lo convertimos a minusculas para que el filtro no distinga mayúsculas/minusculas.

                QString nombreUsuario = nombreLabel->text().remove("Nombre de usuario: ").toLower();
                //aqui si el texto ingresado (tambien en minusculas) esta contenido dentro del nombre de usuario,
                //→ entonces setVisible(true) → se muestra.
                //aqui si no lo contiene, → setVisible(false) → se oculta.
                card->setVisible(nombreUsuario.contains(texto.toLower()));

            }
        }

        //En resumen

        //connect(..., textChanged, ...)	Ejecuta una funciion cada vez que cambia el texto del campo de busqueda.

        //findChild("lblNombreUsuario")	Busca dentro de cada tarjeta el QLabel con el nombre del usuario

        //.remove("Nombre de usuario: ")	Limpia el texto para quedarte solo con el nombre

        //contains(texto.toLower())	Compara si el nombre incluye lo que escribiste (ignorando mayusculas).

        //card->setVisible(...)	Muestra u oculta la tarjeta segun si coincide con lo buscado.

    });

    return panel;
}
QWidget *MainWindow::crearPanelContactos()
{

    QWidget *panel=new QWidget();
    panel->setStyleSheet("background-color: white");
    return panel;

}

QWidget *MainWindow::crearPanelMensajes()
{

    QWidget *panel=new QWidget();
    panel->setStyleSheet("background-color: white");
    return panel;

}




