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
#include"gestornotificaciones.h"
#include<QTimer>
#include"piladeshacer.h"
#include<QStringList>
#include<QRegularExpression>
#include<QRegularExpressionMatch>
#include<QCoreApplication>
#include<QFile>
#include<QScrollBar>
#include"Mensaje.h"
#include<functional>
#include"colanoleidos.h"
#include"ClickableLabel.h"
#include<QComboBox>


MainWindow::MainWindow(Usuario usuario,QWidget *parent)
    : QMainWindow(parent)
    , UsuarioActivo(usuario)

{

    //aqui crea carpeta de stickers disponibles
    QString rutaBase = QDir::currentPath();
    QDir carpetaStickersDisponibles(rutaBase + "/stickers_disponibles");
    if (!carpetaStickersDisponibles.exists()) {
        carpetaStickersDisponibles.mkpath(".");
    }

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
    btnHistorial = new QPushButton(QIcon(rutaImagenes+"historial.png")," Historial",this);
    btnBuscar = new QPushButton(QIcon(rutaImagenes+"buscar.png")," Buscar",this);
    btnMensajes = new QPushButton(QIcon(rutaImagenes+"mensajes.png")," Mensajes",this);
    btnVentanaStickers = new QPushButton(QIcon(rutaImagenes+"stickers.png")," Stickers",this);
    btnNotificaciones = new QPushButton(QIcon(rutaImagenes+"buzon.png")," Buzón",this);
    ActualizarContadorNotificaciones();
    btnCerrarSesion = new QPushButton(QIcon(rutaImagenes+"cerrar.png"),"Cerrar Sesión",this);

    QString EstiloBoton="QPushButton {background-color: transparent; color: white;  font-size: 16px; padding: 12px; text-align: left;}"
                          "QPushButton:hover {background-color: #40739e; }";

    btnHistorial->setStyleSheet(EstiloBoton);
    btnBuscar->setStyleSheet(EstiloBoton);
    btnVentanaStickers->setStyleSheet(EstiloBoton);
    btnMensajes->setStyleSheet(EstiloBoton);
    btnNotificaciones->setStyleSheet(EstiloBoton);
    btnCerrarSesion->setStyleSheet(EstiloBoton);

    //Aqui se crean los botones del lado derecho la parte azulita
    layoutLateral->addWidget(lblAvatar);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(lblNombre);
    layoutLateral->addStretch();
    layoutLateral->addWidget(btnHistorial);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnBuscar);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnVentanaStickers);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnMensajes);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnNotificaciones);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnCerrarSesion);


    CargarEstadoMensajes();
    CargarBorradores();

    // ======= aqui se crea el panel derecho ======
    paneles=new QStackedWidget(this);
    paneles->addWidget(crearPanelBuscar());//aqui con el indiice 0
    paneles->addWidget(crearPanelStickers());//aqui con el indice 1
    paneles->addWidget(crearPanelMensajes());//aqui con el indice 2
    paneles->addWidget(crearPanelNotificaciones());//aqui nuevo indice 3
    paneles->addWidget(crearPanelHistorial());//aqui nuevo indice 4

    for (auto it = colasNoLeidos.begin(); it != colasNoLeidos.end(); ++it)
    {
        qDebug() << "Usuario:" << it.key() << " Mensajes no leidosssssssssssssssssaj sjas ajs j:" << it.value().size();
    }

    //aqui se crea el layout principal
    QHBoxLayout *layoutPrincipal=new QHBoxLayout(central);
    layoutPrincipal->addWidget(barraLateral);
    layoutPrincipal->addWidget(paneles);

    // =======CONEXIONES======
    connect(btnHistorial,&QPushButton::clicked,this,&MainWindow::mostrarPanelHistorial);
    connect(btnBuscar,&QPushButton::clicked,this,&MainWindow::mostrarPanelBuscar);
    connect(btnVentanaStickers,&QPushButton::clicked,this,&MainWindow::mostrarPanelStickers);
    connect(btnMensajes,&QPushButton::clicked,this,&MainWindow::mostrarPanelMensajes);
    connect(btnNotificaciones, &QPushButton::clicked, this, [=]()
            {

                //aqui marcar como vistas inmediatamente
                GestorNotificaciones gestor;
                gestor.MarcarComoVistaBuzon(UsuarioActivo.getUsuario());

                //aqui actualizar el contador visual antes de abrir el panel
                ActualizarContadorNotificaciones();

                //aqui ahora si mostramos el panel
                mostrarPanelNotificaciones();

            });
    connect(btnCerrarSesion,&QPushButton::clicked,this,&MainWindow::cerrarSesion);


    //aqui se mostrara el panel por defecto
    paneles->setCurrentIndex(0);

    // Timer que detecta nuevos mensajes reales
    QTimer *timerNuevos = new QTimer(paneles);
    timerNuevos->start(2000);
    connect(timerNuevos, &QTimer::timeout, this, [=]() {
        int total = 0;
        QStringList amigos=gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario());
        for (const QString& usuario : amigos)
        {

            if (usuario==UsuarioActivo.getUsuario()) continue;

            QStringList mensajes = CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), usuario);
            int totalMensajes = mensajes.size();
            int previos = this->ultimosMensajesContados.value(usuario, 0);
            int nuevos = totalMensajes - previos;

            if (nuevos > 0 && usuario != this->chatActivo)
            {

                // Insertar en cola
                for (int i=0;i < nuevos; ++i)
                    this->colasNoLeidos[usuario].Insertar(Mensaje<QString>());

                //aqui actualiza la etiqueta del contacto
                if (this->etiquetasContador.contains(usuario))
                {

                    this->etiquetasContador[usuario]->setText(QString::number(this->colasNoLeidos[usuario].size()));
                    this->etiquetasContador[usuario]->setVisible(true);

                }

            }

            // Actualizar conteo interno
            this->ultimosMensajesContados[usuario] = totalMensajes;
            total+=this->colasNoLeidos[usuario].size();

        }

        // Mostrar mensaje solo si NO estamos en panel de mensajes
        if(total>0&&paneles->currentIndex()!=2)
        {

            this->btnMensajes->setText("Mensajes - ¡Nuevos mensajes!");

        }else if(total==0){

            this->btnMensajes->setText("Mensajes");

        }
    });

    QTimer *timerContador=new QTimer(this);
    connect(timerContador,&QTimer::timeout,this,[=]()
            {

                ActualizarContadorNotificaciones();

            });
    timerContador->start(2000); // cada 2 segundos

}

// =====AQUI FUNCIONES PARA CAMBIAR DE PANEL=====
void MainWindow::mostrarPanelBuscar()
{

    QWidget*viejo=paneles->widget(0);
    QWidget*nuevo=crearPanelBuscar();
    paneles->removeWidget(viejo);
    delete viejo;
    paneles->insertWidget(0,nuevo);
    paneles->setCurrentIndex(0);

}

void MainWindow::mostrarPanelStickers()
{

    QWidget*viejo=paneles->widget(1);
    QWidget*nuevo=crearPanelStickers();
    paneles->removeWidget(viejo);
    delete viejo;
    paneles->insertWidget(1,nuevo);
    paneles->setCurrentIndex(1);

}

void MainWindow::mostrarPanelMensajes()
{

    paneles->setCurrentIndex(2);
    btnMensajes->setText("Mensajes");

}

void MainWindow::mostrarPanelNotificaciones()
{

    QWidget*viejo=paneles->widget(3);
    QWidget*nuevo=crearPanelNotificaciones();
    paneles->removeWidget(viejo);
    delete viejo;
    paneles->insertWidget(3,nuevo);
    paneles->setCurrentIndex(3);

}

void MainWindow::mostrarPanelHistorial()
{


    historialCompleto = CargarMensajesHistorial();
    MostrarMensajesEnLista(historialCompleto);
    paneles->setCurrentIndex(4);

}

void MainWindow::cerrarSesion()
{

    GuardarBorradores();
    GuardarEstadoMensajes();
    MarcarUsuarioComoDesconectado(UsuarioActivo.getUsuario());
    Inicio *i=new Inicio();
    i->show();
    this->close();

}

// ====AQUI FUNCIONES PARA CREAR LOS PANELES=====
//========================================BUSCADOR======================================================================
//========================================BUSCADOR======================================================================
//========================================BUSCADOR======================================================================
//========================================BUSCADOR======================================================================
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
    QMap<QString, QPushButton*> mapaBotonesSolicitud;

    QList<Usuario> usuarios=CargarUsuarios();
    //aqui ordenamiento burbuja, para ordenar la lista de contacto en orden alfabetico

    //se usa un doble for, el externo recorre todo el arreglo, el interno compara pares adyacentes de usuarios (usuarios[j] y usuarios[j+1]).

    //los convierte en minusculas para que la comparacion no sea sensible a mayusculas y minusculas

    //si el usuario actual va despues alfabaticamente que el siguiente nombre1 > nombre2 los intercambia

    for(int i=0;i<usuarios.size()-1;++i)
    {

        for(int j=0;j<usuarios.size()-i-1;++j)
        {

            QString nombre1=usuarios[j].getNombre().toLower();
            QString nombre2=usuarios[j+1].getNombre().toLower();
            if(nombre1>nombre2)
            {

                Usuario aux=usuarios[j];
                usuarios[j]=usuarios[j+1];
                usuarios[j+1]=aux;

            }

        }

    }

    QMap<QString, QLabel*>mapaEstados;// para guardar los estados en un mapa

    for(const Usuario&u:usuarios) {
        if(u.getUsuario()==UsuarioActivo.getUsuario()) continue;

        // Tarjeta de usuario bro xd
        QWidget *card=new QWidget;
        card->setStyleSheet("background-color: white; border: 1px solid #dcdde1; border-radius: 10px;");
        QHBoxLayout *cardLayout = new QHBoxLayout(card);
        card->setFixedHeight(85);
        card->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);// No se colapsa al ocultar

        // Avatar
        QLabel *avatar=new QLabel;
        QPixmap pix(u.getRutaAvatar());
        avatar->setPixmap(pix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        avatar->setFixedSize(60, 60);
        avatar->setStyleSheet("border-radius: 30px;");

        // Info del usuario
        QLabel *lblNombre=new QLabel(u.getNombre());
        lblNombre->setObjectName("lblNombreUsuario");
        lblNombre->setStyleSheet("font-size: 15px; font-weight: bold; color: #2f3640; border: none; background: transparent;");

        bool conectado=Estaconectado(u.getUsuario());
        QString estadotexto=conectado?"Online":"Offline";
        QString colorestado=conectado?"green":"red";
        QLabel *estado=new QLabel(estadotexto);
        estado->setStyleSheet("color: "+colorestado+"; font-size: 13px; border: none; background: transparent;");
        mapaEstados[u.getUsuario()]=estado;

        QVBoxLayout *infoLayout=new QVBoxLayout;
        infoLayout->addWidget(lblNombre);
        infoLayout->addWidget(estado);
        infoLayout->setSpacing(0);
        infoLayout->setContentsMargins(0,0,0,0);

        //aqui el boton agregar
        QPushButton *btnSolicitud = new QPushButton;
        btnSolicitud->setStyleSheet("QPushButton { background-color: #3498db; color: white; padding: 8px 16px; font-size: 13px; border-radius: 6px; } QPushButton:disabled { background-color: gray; }");
        mapaBotonesSolicitud[u.getUsuario()] = btnSolicitud;

        QTimer *timerBotones=new QTimer(panel);
        connect(timerBotones,&QTimer::timeout, panel,[=]() {
            gestionContactos.CargarDatos(); // Recarga desde archivo actualizado

            for (const Usuario &u : usuarios) {
                if (!mapaBotonesSolicitud.contains(u.getUsuario())) continue;

                QPushButton *btn = mapaBotonesSolicitud[u.getUsuario()];
                if (!btn) continue;

                // ORDEN CORRECTO
                if(gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario()).contains(u.getUsuario()))
                {

                    btn->setText("Amigos");
                    btn->setEnabled(false);

                }else if(gestionContactos.TieneSolicitudRecibida(UsuarioActivo.getUsuario(), u.getUsuario())) {

                    btn->setText("Solicitud recibida");
                    btn->setEnabled(false);

                }else if(gestionContactos.ObtenerSolicitudesPendientes(u.getUsuario()).contains(UsuarioActivo.getUsuario())) {

                    btn->setText("Solicitud enviada");
                    btn->setEnabled(false);

                }else{

                    btn->setText("Agregar");
                    btn->setEnabled(true);

                }
            }
        });
        timerBotones->start(1000); // Actualiza cada 3 segundos

        connect(btnSolicitud, &QPushButton::clicked,[=]()
                {

                    if(gestionContactos.EnviarSolicitud(UsuarioActivo.getUsuario(), u.getUsuario()))
                    {
                        //aqui se crea la notificaion'
                        Notificacion noti(u.getUsuario(), UsuarioActivo.getUsuario(), "solicitud", "", "nueva");//aqui el mensaje se construye en el panel
                        GestorNotificaciones gestor;
                        gestor.agregarnotificacion(noti);

                        ActualizarContadorNotificaciones();

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
        contenedorUsuarios->setMinimumHeight(tarjetasUsuarios.size()*85+20);//aqui altura totl
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

            });

    QTimer *timer=new QTimer(panel);
    connect(timer,&QTimer::timeout,panel,[=](){

        for(auto messi=mapaEstados.begin();messi!=mapaEstados.end();++messi)
        {

            QString usuario=messi.key();
            QLabel *estadoLabel=messi.value();
            bool conectado=Estaconectado(usuario);
            estadoLabel->setText(QString(conectado ? "Online" : "Offline"));
            estadoLabel->setStyleSheet("color: " + QString(conectado ? "green" : "red") + "; font-size: 13px; border: none; background: transparent;");
        }
    });
    timer->start(2000); // Actualiza cada 3 segundos

    //aqui actualizacion de botones de solicitud cada 3 segundos
    QTimer *timerBotones=new QTimer(panel);
    connect(timerBotones, &QTimer::timeout, panel, [=]() {
        gestionContactos.CargarDatos(); // para que se actualicen solicitudes y contactos
        for (const Usuario &u : usuarios)
        {
            if(!mapaBotonesSolicitud.contains(u.getUsuario())) continue;

            QPushButton *btn=mapaBotonesSolicitud[u.getUsuario()];
            if(!btn)continue;

            if(gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario()).contains(u.getUsuario()))
            {

                btn->setText("Amigos");
                btn->setEnabled(false);

            }else if(gestionContactos.ObtenerSolicitudesPendientes(u.getUsuario()).contains(UsuarioActivo.getUsuario())){

                btn->setText("Solicitud enviada");
                btn->setEnabled(false);

            }else{

                btn->setText("Agregar");
                btn->setEnabled(true);

            }
        }
    });
    timerBotones->start(2000);

    int cantidadUsuariosINicial=usuarios.size();

    QTimer *timerUsuariosNuevos=new QTimer(panel);
    connect(timerUsuariosNuevos,&QTimer::timeout,this,[=](){

        QList<Usuario> listaActualizada = CargarUsuarios();
        if(listaActualizada.size()!=cantidadUsuariosINicial)
        {

            //aqui reconstruir todo el panel si se detecta un nuevo usuario
            QWidget *nuevo=crearPanelBuscar();
            int indice=paneles->indexOf(panel);
            paneles->removeWidget(panel);
            panel->deleteLater();
            paneles->insertWidget(indice, nuevo);
            paneles->setCurrentIndex(indice);

        }

    });
    timerUsuariosNuevos->start(2000); // cada 3 segundos revisa si hay nuevos usuarios

    return panel;
}

//========================================STICKERS======================================================================
//========================================STICKERS======================================================================
//========================================STICKERS======================================================================
//========================================STICKERS======================================================================
QWidget *MainWindow::crearPanelStickers()
{

    QWidget *panel=new QWidget();
    panel->setStyleSheet("background-color: white");
    QVBoxLayout*layoutPanel=new QVBoxLayout(panel);
    layoutPanel->setContentsMargins(20,20,20,20);

    QLabel*titulo=new QLabel("Explorar Stickers");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold; color: #2f3e50;");
    layoutPanel->addWidget(titulo);

    //aqui area del scroll
    QScrollArea*scroll=new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget*contenedor=new QWidget;
    QGridLayout*grid=new QGridLayout(contenedor);
    grid->setSpacing(20);
    grid->setContentsMargins(10,10,10,10);

    //aqui lee stickers desde la carpeta
    QDir carpeta(obtenerRutaStickersDisponibles());
    QString rutaArchivo=obtenerRutaFavoritos(UsuarioActivo.getUsuario());
    QStringList listaStickers=carpeta.entryList(QStringList()<<"*.png"<<"*.jpg",QDir::Files);

    //aqui se leen los stickes favoritos del usuario
    QSet<QString>favoritos;//aqui el set se utiliza para almacenar elementos unicos y ordenados

    QFile ArchivoFavoritos(rutaArchivo);
    if(ArchivoFavoritos.open(QIODevice::ReadOnly|QIODevice::Text))
    {


        //aqui el atEnd se utiliza para comprobar si un iterador ha llegado al final de un contenedor.
        //Devolvera true si el iterador ha alcanzado el elemento final del contenedor y false en caso contrario.
        QTextStream in(&ArchivoFavoritos);
        while(!in.atEnd())
        {

            QString linea=in.readLine().trimmed();
            if(!linea.isEmpty())
            {

                favoritos.insert(linea);//insert sera como pilas y colas? hmmmmm

            }

        }
        ArchivoFavoritos.close();

    }


    int fila=0,columna=0;
    for(const QString &nombre:listaStickers)
    {

        QString ruta=carpeta.filePath(nombre);

        QWidget *card=new QWidget;
        QVBoxLayout*cardLayout=new QVBoxLayout(card);

        QLabel *img=new QLabel;
        img->setPixmap(QPixmap(ruta).scaled(100,100,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        img->setAlignment(Qt::AlignCenter);

        QLabel*etiqueta=new QLabel(nombre);
        etiqueta->setAlignment(Qt::AlignCenter);
        etiqueta->setStyleSheet("color: #7f8c8d; font-size: 12px;");

        //aqui se crea el boton favorito
        QPushButton *btnFavorito = new QPushButton;
        btnFavorito->setMaximumWidth(100);
        btnFavorito->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        btnFavorito->setStyleSheet(
            "font-size: 12px;"
            "color: black;"
            "background-color: #f1c40f;"
            "border: none;"
            "border-radius: 6px;"
            "padding: 6px 12px;"
            "min-height: 24px;"
            );

        if(favoritos.contains(nombre))
        {

            btnFavorito->setText("❌ Quitar");
            connect(btnFavorito,&QPushButton::clicked,this,[=](){

                //aqui lee todo el archivo y elimina linea del sticker
                QFile Archivo(rutaArchivo);
                QStringList lineas;

                if(Archivo.open(QIODevice::ReadOnly|QIODevice::Text))
                {

                    QTextStream in(&Archivo);
                    while(!in.atEnd())
                    {

                        QString linea=in.readLine().trimmed();
                        if(linea!=nombre&&!linea.isEmpty())
                        {

                            lineas<<linea;

                        }

                    }
                    Archivo.close();

                }

                //aqui escribir el archivo sin ese sticker
                if(Archivo.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                {

                    QTextStream out(&Archivo);
                    for(const QString &linea:lineas)
                    {

                        out<<linea<<"\n";

                    }
                    Archivo.close();

                    btnFavorito->setText("✅ Agregar");
                    btnFavorito->disconnect();//aqui desconectamos todas las señales previas

                    connect(btnFavorito,&QPushButton::clicked,this,[=]()
                            {

                                QFile archivo(rutaArchivo);
                                if(archivo.open(QIODevice::Append|QIODevice::Text))
                                {

                                    QTextStream out(&archivo);
                                    out<<nombre<<"\n";
                                    archivo.close();

                                    btnFavorito->setText("❌ Quitar");

                                }


                            });

                }

            });

        }else{

            btnFavorito->setText("✅ Agregar");
            connect(btnFavorito,&QPushButton::clicked,this,[=](){

                QFile archivo(rutaArchivo);
                if(archivo.open(QIODevice::Append|QIODevice::Text))
                {

                    QTextStream out(&archivo);
                    out<<nombre<<"\n";
                    archivo.close();

                    btnFavorito->setText("❌ Quitar");
                    btnFavorito->disconnect();

                    connect(btnFavorito,&QPushButton::clicked,this,[=](){

                        //aqui elimina al hacer clicl en quitar
                        QFile archivo(rutaArchivo);
                        QStringList lineas;

                        if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
                        {

                            QTextStream in(&archivo);
                            while(!in.atEnd())
                            {

                                QString linea=in.readLine().trimmed();
                                if(linea!=nombre&&!linea.isEmpty())
                                {

                                    lineas<<linea;

                                }

                            }
                            archivo.close();

                        }
                        if(archivo.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                        {

                            QTextStream out(&archivo);
                            for(const QString &linea:lineas)
                            {

                                out<<linea<<"\n";

                            }
                            archivo.close();

                            btnFavorito->setText("✅ Agregar");
                            btnFavorito->disconnect();

                            connect(btnFavorito,&QPushButton::clicked,this,[=](){

                                // y volver a agregar
                                QFile archivo(rutaArchivo);
                                if(archivo.open(QIODevice::Append|QIODevice::Text))
                                {

                                    QTextStream out(&archivo);
                                    out<<nombre<<"\n";
                                    btnFavorito->setText("❌ Quitar");


                                }

                            });

                        }

                    });

                }

            });

        }

        QHBoxLayout*centrado=new QHBoxLayout;
        centrado->addStretch();
        centrado->addWidget(btnFavorito);
        centrado->addStretch();

        cardLayout->addWidget(img);
        cardLayout->addWidget(etiqueta);
        cardLayout->addLayout(centrado);

        card->setStyleSheet("background-color: #ecf0f1; border-radius: 10px;");
        grid->addWidget(card,fila,columna);

        columna++;
        if(columna==4)//4 columnas por fila
        {

            columna=0;
            fila++;

        }
        scroll->setWidget(contenedor);
        layoutPanel->addWidget(scroll);


    }


    return panel;

}
//========================================MENSAJES======================================================================
//========================================MENSAJES======================================================================
//========================================MENSAJES======================================================================
//========================================MENSAJES======================================================================
QWidget *MainWindow::crearPanelMensajes()
{

    QWidget *panel=new QWidget();
    panel->setStyleSheet("background-color: #ecf0f1;");
    QHBoxLayout*mainlayout=new QHBoxLayout(panel);

    //aqui lista izquierda de contactos
    QListWidget*listacontactos=new QListWidget;
    listacontactos->setFixedWidth(250);
    listacontactos->setStyleSheet("background-color: white; font-size: 14px; border-right: 1px solid #dcdde1;");
    mainlayout->addWidget(listacontactos);

    //aqui panel derecho para el chat
    QWidget*panelchat=new QWidget;
    panelchat->setStyleSheet("background-color: white;");
    QVBoxLayout*chatlayout=new QVBoxLayout(panelchat);
    QLabel*posicion=new QLabel("Selecciona un contacto para comenzar a chatear");
    posicion->setStyleSheet("font-size: 16px; color: #7f8c8d;");
    chatlayout->addWidget(posicion);
    mainlayout->addWidget(panelchat);


    QStringList amigos = gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario());
    for (const QString &amigo : amigos)
    {

        if (!this->ultimosMensajesContados.contains(amigo))
        {

            this->ultimosMensajesContados[amigo] = CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), amigo).size();

        }

    }
    this->chatActivo = ""; // ningun chat abierto al inicio

    this->btnMensajes->setText("Mensajes");

    //AQUI SE MUESTRAN LOS CONTACTOS
    //aqui se obtienen todos los usuarios y amigos
    QList<Usuario>usuarios=CargarUsuarios();

    //aqui se mostraran cada amigo con su avatar y nombre completo
    QMap<QString, QLabel*> mapaPuntosEstado; // Para actualizar los puntos en tiempo real

    for(const QString &amigoUsuario:amigos)
    {

        for(const Usuario &u:usuarios)
        {

            if(u.getUsuario()==amigoUsuario)
            {

                //aqui widget personalizado
                QWidget*itemWidget=new QWidget;
                itemWidget->setStyleSheet(
                    "background-color: #f9f9f9;"
                    "border: 1px solid #dcdde1;"
                    "border-radius: 6px;"
                    );


                QHBoxLayout*layout=new QHBoxLayout(itemWidget);
                layout->setContentsMargins(10,5,10,5);

                //aqui avatar
                QLabel *avatarLabel=new QLabel;
                QPixmap avatar(u.getRutaAvatar());
                avatarLabel->setPixmap(avatar.scaled(40,40,Qt::KeepAspectRatio,Qt::SmoothTransformation));
                avatarLabel->setFixedSize(40,40);

                QLabel *puntoEstado=new QLabel;//PARA SABER SI ESTA ONLINE O OFFLINE EL USUARIO
                puntoEstado->setFixedSize(12,12);
                puntoEstado->setStyleSheet("border-radius: 6px;");
                bool conectado=Estaconectado(u.getUsuario());
                puntoEstado->setStyleSheet("background-color: " + QString(conectado ? "green" : "red") + "; border-radius: 6px;");

                //aqui nombre completo
                QLabel *nombreLabel=new QLabel(u.getNombre());
                nombreLabel->setStyleSheet("font-size: 14px; color: #2c3e50; font-weight: bold;");

                //AQUI EL BOTON DE ELIMINAR CONTACTO
                QPushButton* btnEliminar=new QPushButton(this);
                btnEliminar->setIcon(QIcon(rutaImagenes+"delete.png"));
                btnEliminar->setFixedSize(24,24);
                btnEliminar->setIconSize(QSize(20,20));
                btnEliminar->setToolTip("Eliminar contacto");
                btnEliminar->setCursor(Qt::PointingHandCursor);
                btnEliminar->setStyleSheet(
                    "QPushButton { background-color: transparent; border: none; }"
                    "QPushButton:hover { background-color: #e74c3c; border-radius: 6px; }"
                    );

                //aqui conexion al eliminar
                connect(btnEliminar, &QPushButton::clicked, this, [=]()
                {

                    QString nombreCompleto=u.getNombre();
                    QString usuarioEliminar=u.getUsuario();

                    QMessageBox::StandardButton respuesta=QMessageBox::question(this,"Confirmar eliminacion","¿Estas seguro de eliminar a \""+nombreCompleto+"\"?\nSe perdera todo el historial del chat.",QMessageBox::Yes|QMessageBox::No);

                    if(respuesta == QMessageBox::Yes)
                    {

                        // Paso 1: Eliminar del archivo contactos
                        gestionContactos.EliminarContacto(UsuarioActivo.getUsuario(), usuarioEliminar);
                        GestorNotificaciones::EliminarNotificacionesEntre(UsuarioActivo.getUsuario(),usuarioEliminar);
                        EliminarSolicitud(UsuarioActivo.getUsuario(),usuarioEliminar);


                        // Paso 2: Vaciar archivo de conversacion
                        auto obtenerRutaArchivo=[](const QString &usuario1, const QString &usuario2)
                        {

                            QDir dir(QCoreApplication::applicationDirPath());
                            dir.cdUp();
                            QString nombre1=usuario1.toLower();
                            QString nombre2=usuario2.toLower();
                            if (nombre1>nombre2)std::swap(nombre1,nombre2);
                            return dir.filePath("conversaciones/"+nombre1+"_"+nombre2+".txt");

                        };

                        QString rutaConversacion=obtenerRutaArchivo(UsuarioActivo.getUsuario(), usuarioEliminar);
                        QFile archivo(rutaConversacion);
                        if(archivo.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text))
                        {

                            archivo.close(); // Vaciar el contenido

                        }

                        // Paso 3: Eliminar solicitudes.txt entre ambos
                        QString rutaSolicitudes=QCoreApplication::applicationDirPath() + "/../solicitudes.txt";
                        QFile archivoSolicitudes(rutaSolicitudes);
                        if(archivoSolicitudes.open(QIODevice::ReadOnly | QIODevice::Text))
                        {

                            QStringList lineasNuevas;
                            QTextStream in(&archivoSolicitudes);
                            while (!in.atEnd()) {
                                QString linea = in.readLine();
                                QStringList partes = linea.split("|");
                                if (partes.size() == 3) {
                                    QString remitente = partes[0];
                                    QString destinatario = partes[1];
                                    if (!((remitente == UsuarioActivo.getUsuario() && destinatario == usuarioEliminar) ||
                                          (remitente == usuarioEliminar && destinatario == UsuarioActivo.getUsuario()))) {
                                        lineasNuevas << linea;
                                    }
                                }
                            }
                            archivoSolicitudes.close();
                            if (archivoSolicitudes.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                                QTextStream out(&archivoSolicitudes);
                                for (const QString &l : lineasNuevas) out << l << "\n";
                                archivoSolicitudes.close();
                            }
                        }

                        // Paso 4: Eliminar notificaciones.txt relacionadas
                        QString rutaNotificaciones = QCoreApplication::applicationDirPath() + "/../notificaciones.txt";
                        QFile archivoNotificaciones(rutaNotificaciones);
                        if (archivoNotificaciones.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QStringList lineasNuevas;
                            QTextStream in(&archivoNotificaciones);
                            while (!in.atEnd()) {
                                QString linea = in.readLine();
                                QStringList partes = linea.split("|");
                                if (partes.size() == 5) {
                                    QString destino = partes[0];
                                    QString relacionado = partes[1];
                                    // Eliminar si involucra a ambos usuarios en cualquier orden
                                    if (!((destino == UsuarioActivo.getUsuario() && relacionado == usuarioEliminar) ||
                                          (destino == usuarioEliminar && relacionado == UsuarioActivo.getUsuario()))) {
                                        lineasNuevas << linea;
                                    }
                                }
                            }
                            archivoNotificaciones.close();
                            if (archivoNotificaciones.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                                QTextStream out(&archivoNotificaciones);
                                for (const QString &l : lineasNuevas) out << l << "\n";
                                archivoNotificaciones.close();
                            }
                        }

                        // Paso 5: Recargar interfaz de mensajes
                        QWidget *nuevo = crearPanelMensajes();
                        int index = paneles->indexOf(paneles->currentWidget());
                        QWidget *actual = paneles->currentWidget();
                        paneles->removeWidget(actual);
                        actual->deleteLater();
                        paneles->insertWidget(index, nuevo);
                        paneles->setCurrentIndex(index);
                    }
                });

                // layout horizontal para punto + nombre
                QHBoxLayout *nombreLayout = new QHBoxLayout;
                nombreLayout->setContentsMargins(0,0,0,0);
                nombreLayout->setSpacing(6); // espacio entre punto y texto
                nombreLayout->addWidget(puntoEstado);
                nombreLayout->addWidget(nombreLabel);

                QWidget *nombreWidget = new QWidget;
                nombreWidget->setLayout(nombreLayout);

                layout->addWidget(avatarLabel);
                layout->addWidget(nombreWidget); // agregas el widget con el nombre y el punto
                layout->addWidget(btnEliminar);
                layout->addStretch();


                //aqui crea y agregar el contador de mensajes no leidos
                QLabel* etiquetaContador = new QLabel;
                etiquetaContador->setStyleSheet("background-color: red; color: white; font-weight: bold; padding: 2px 6px; border-radius: 10px; font-size: 13px;");
                etiquetaContador->setFixedSize(24, 20);
                etiquetaContador->setAlignment(Qt::AlignCenter);

                //aqio obtiene la cantidad de mensajes no leidos cargados desde archivo (colasNoLeidos)
                int cantidad = 0;
                if (this->colasNoLeidos.contains(u.getUsuario()))
                {

                    cantidad = this->colasNoLeidos[u.getUsuario()].size();

                }else{

                    this->colasNoLeidos[u.getUsuario()] = ColaNoLeidos<Mensaje<QString>>();
                }

                etiquetaContador->setText(QString::number(cantidad));
                etiquetaContador->setVisible(cantidad > 0);

                // Añadir al layout visual
                layout->addWidget(etiquetaContador);

                // Guardar en mapa
                this->etiquetasContador[u.getUsuario()] = etiquetaContador;
                if (this->colasNoLeidos.contains(u.getUsuario())) {
                    int cantidad = this->colasNoLeidos[u.getUsuario()].size();
                    etiquetaContador->setText(QString::number(cantidad));
                    etiquetaContador->setVisible(cantidad > 0);
                }

                //aqui se crea el item
                QListWidgetItem*item=new QListWidgetItem;
                item->setSizeHint(QSize(0,60));  //aqui mayor altura para mejor visibilidad
                item->setData(Qt::UserRole,u.getUsuario()); //aqui guarda el usuario como identificador

                listacontactos->addItem(item);
                listacontactos->setItemWidget(item, itemWidget);

                mapaPuntosEstado[u.getUsuario()]=puntoEstado;

                break;

            }

        }

    }

    static bool MensajeEliminado=false;
    QTimer *TimerMensajes=new QTimer(panel);//aqui se crea una sola vez
    TimerMensajes->start(2000);

    //aqui la conexion al hacer click sobre un contacto
    connect(listacontactos,&QListWidget::itemClicked,[=](QListWidgetItem*item)
            {

                //esto servira para poder remarcar el usuario con quien se mensajea
                if(itemContactoSeleccionado)
                {

                    QWidget* anterior=listacontactos->itemWidget(itemContactoSeleccionado);
                    if(anterior)
                    {

                        anterior->setStyleSheet(
                            "background-color: #f9f9f9;"
                            "border: 1px solid #dcdde1;"
                            "border-radius: 6px;");

                    }

                }

                QWidget* actual = listacontactos->itemWidget(item);
                if(actual)
                {

                    actual->setStyleSheet(
                        "background-color: #d6eaf8;"
                        "border: 2px solid #3498db;"
                        "border-radius: 6px;");

                }
                itemContactoSeleccionado=item;

                QString usuamigo=item->data(Qt::UserRole).toString();

                this->chatActivo = usuamigo;

                // Guardar mensaje actual antes de cambiar de contacto
                if (!chatActivo.isEmpty()) {
                    QList<QLineEdit*> campos = panelchat->findChildren<QLineEdit*>();
                    if (!campos.isEmpty()) {
                        QLineEdit* campoActual = campos.last();
                        borradoresMensajes[chatActivo] = campoActual->text();
                    }
                }

                // Reset de cola y etiqueta visual
                if (this->colasNoLeidos.contains(usuamigo))
                {

                    this->colasNoLeidos[usuamigo] = ColaNoLeidos<Mensaje<QString>>();

                }
                if(this->etiquetasContador.contains(usuamigo))
                {

                    this->etiquetasContador[usuamigo]->setVisible(false);

                }

                //aqui se considera como leidos todos los mensajes actuales
                this->ultimosMensajesContados[usuamigo]=CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), usuamigo).size();
                this->btnMensajes->setText("Mensajes");

                //aqui se busca nombre completoo del amigo
                QString nombrecompleto;
                for(const Usuario &u:usuarios)
                {

                    if(u.getUsuario()==usuamigo)
                    {

                        nombrecompleto=u.getNombre();
                        break;

                    }
                }

                //aqui se limpia el chat anterior
                QLayoutItem *w;
                while((w=chatlayout->takeAt(0))!=nullptr)
                {

                    if(w->widget())delete w->widget();
                    delete w;

                }

                //aqui cabecera de chat
                QLabel *cabecera=new QLabel("Chat con: "+nombrecompleto);
                cabecera->setStyleSheet("font-weight: bold; font-size: 18px;");
                chatlayout->addWidget(cabecera);

                //aqui fecha de inicio de la conversacion
                QString fecha=QDate::currentDate().toString("dddd dd MMMM yyyy");
                QLabel*fechaInicio=new QLabel(fecha);
                fechaInicio->setStyleSheet("color: gray; font-size: 12px; margin-bottom: 10px;");
                chatlayout->addWidget(fechaInicio);

                //aqui area scroll de mensajes
                QScrollArea *scrollArea = new QScrollArea;
                QWidget *contenedorMensajes = new QWidget;
                layoutMensajes = new QVBoxLayout;
                contenedorMensajes->setLayout(layoutMensajes);
                scrollArea->setWidgetResizable(true);
                scrollArea->setWidget(contenedorMensajes);
                chatlayout->addWidget(scrollArea);

                //aqui se crea el campo de mensaje
                QLineEdit *entradaMensaje = new QLineEdit;
                entradaMensaje->setText(borradoresMensajes.value(usuamigo,""));
                entradaMensaje->setPlaceholderText("Escribe un mensaje...");
                entradaMensaje->setStyleSheet("color: black; font-size: 14px; padding: 6px;");

                QPushButton *btnEnviar = new QPushButton("Enviar");
                btnEnviar->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");

                QPushButton *btnSticker = new QPushButton("Stickers");
                btnSticker->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");

                connect(btnSticker,&QPushButton::clicked,this,[=](){

                    mostrarPanelStickersFavoritos();

                });

                QPushButton *btnDeshacer=new QPushButton("Deshacer ultimo mensaje");
                btnDeshacer->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");

                QPushButton *btnRestaurar=new QPushButton("Restaurar ultimo mensaje");
                btnRestaurar->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");


                QHBoxLayout *botonesLayout = new QHBoxLayout;
                botonesLayout->addWidget(btnEnviar);
                botonesLayout->addWidget(btnSticker);
                botonesLayout->addWidget(btnDeshacer);
                botonesLayout->addWidget(btnRestaurar);

                chatlayout->addWidget(entradaMensaje);
                chatlayout->addLayout(botonesLayout);

                std::function<void()> actualizarMensajes;

                actualizarMensajes = [=]() mutable
                {
                    QVector<InfoMensaje> botonesPendientes;
                    layoutMensajes->setAlignment(Qt::AlignTop);
                    QScrollBar* scrollBar=scrollArea->verticalScrollBar();
                    bool EstaAlFinal=(scrollBar->value()==scrollBar->maximum());
                    QLayoutItem *child;
                    while((child=layoutMensajes->takeAt(0))!=nullptr)
                    {

                        if(child->widget()) child->widget()->deleteLater();
                        delete child;

                    }


                    QStringList mensajes = CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), usuamigo);
                    QString fechaAnterior;

                    //AQUI BACKEND DE COMO SALDRA LOS MENSAJES EN EL CHAT
                    for (const QString &linea : mensajes)
                    {

                        QRegularExpression re(R"(\[(\d{4}-\d{2}-\d{2}) (\d{2}:\d{2})\] ([^ ]+) \(([^)]+)\): (.+))");
                        QRegularExpressionMatch match=re.match(linea);

                        if (match.hasMatch())
                        {

                            QString fecha=match.captured(1);
                            QString hora=match.captured(2);
                            QString emisor=match.captured(3);
                            QString tipo=match.captured(4);
                            QString contenido=match.captured(5);

                            if (fecha!=fechaAnterior)
                            {

                                QLabel *fechaLabel = new QLabel(QDate::fromString(fecha, "yyyy-MM-dd").toString("dd MMMM yyyy"));
                                fechaLabel->setAlignment(Qt::AlignCenter);
                                fechaLabel->setStyleSheet("color: gray; font-size: 12px; margin: 10px;");
                                layoutMensajes->addWidget(fechaLabel);
                                fechaAnterior = fecha;

                            }

                            QLabel*nombreEmisor=new QLabel(emisor);
                            nombreEmisor->setStyleSheet("font-size: 10px; color: gray;");
                            nombreEmisor->setAlignment(emisor==UsuarioActivo.getNombre()?Qt::AlignRight:Qt::AlignLeft);

                            //LOGICA DE COMO SALDRA EL STICKER EN EL CHAT
                            if (tipo=="sticker")
                            {

                                QLabel *sticker=new QLabel;
                                QString rutaCompleta=obtenerRutaStickersDisponibles()+"/"+contenido;
                                if(QFile::exists(rutaCompleta))
                                {

                                    sticker->setPixmap(QPixmap(rutaCompleta).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                                    sticker->setStyleSheet("background-color: transparent; border-radius: 10px;");
                                    sticker->setAlignment(emisor == UsuarioActivo.getUsuario() ? Qt::AlignRight : Qt::AlignLeft);

                                }else{

                                    sticker->setText("[Sticker no encontrado]");

                                }

                                //aqui se crea la etique la hora
                                QLabel*horaLabel=new QLabel(hora);
                                horaLabel->setStyleSheet("font-size: 10px; color: gray; font-weight: bold;");
                                horaLabel->setAlignment(emisor==UsuarioActivo.getUsuario()?Qt::AlignRight:Qt::AlignLeft);

                                //aqui se crea el contenedor del sticker y hora
                                QVBoxLayout *cont=new QVBoxLayout;
                                cont->addWidget(nombreEmisor);
                                cont->addWidget(sticker);
                                cont->addWidget(horaLabel);
                                cont->setAlignment(emisor==UsuarioActivo.getUsuario()?Qt::AlignRight:Qt::AlignLeft);

                                QWidget *linea=new QWidget;
                                linea->setLayout(cont);
                                layoutMensajes->addWidget(linea);

                            }else{

                                QLabel*msg=new QLabel(contenido);
                                msg->setWordWrap(true);
                                msg->setStyleSheet("padding: 8px 6px; border-radius: 12px; font-size: 14px; font-weight: bold;");

                                QLabel *horaLabel = new QLabel(hora);
                                horaLabel->setStyleSheet("font-size: 10px; color: gray; font-weight: bold;");
                                horaLabel->setAlignment(emisor==UsuarioActivo.getUsuario()?Qt::AlignRight:Qt::AlignLeft);

                                QVBoxLayout*cont=new QVBoxLayout;
                                cont->addWidget(nombreEmisor);
                                cont->addWidget(msg);
                                cont->addWidget(horaLabel);
                                cont->setAlignment(emisor==UsuarioActivo.getUsuario()?Qt::AlignRight:Qt::AlignLeft);

                                if (emisor==UsuarioActivo.getUsuario())//MENSAJE DEL USUARIO ACTUAL
                                {

                                    msg->setStyleSheet(msg->styleSheet() + "background-color: #a3e4d7; color: #1c2833;");
                                    botonesPendientes.append({fecha, hora, emisor, tipo, contenido});

                                }else{//MENSAJE DEL OTRO USUARIO

                                    msg->setStyleSheet(msg->styleSheet() + "background-color: #e1e1e1; color: #2d3436;");

                                }

                                QWidget *lineaWidget=new QWidget;
                                lineaWidget->setLayout(cont);
                                layoutMensajes->addWidget(lineaWidget);
                            }

                        }
                    }

                    if(EstaAlFinal)
                    {

                        scrollBar->setValue(scrollBar->maximum());

                    }

                };

                auto ObtenerRutaArchivo=[](const QString &usuario1,const QString &usuario2)
                {

                    QDir dir(QCoreApplication::applicationDirPath());
                    dir.cdUp();
                    QString nombre1=usuario1.toLower();
                    QString nombre2=usuario2.toLower();
                    if(nombre1>nombre2)std::swap(nombre1,nombre2);
                    return dir.filePath("conversaciones/" + nombre1 + "_" + nombre2 + ".txt");


                };

                // Al presionar enviar
                //aaqui asi capturara todo por copia excepto pila, que se captura por referencia, permitiendose modificarla.
                connect(btnEnviar, &QPushButton::clicked, this, [=]()
                {

                    QString texto = entradaMensaje->text().trimmed();
                    if (texto.isEmpty()) return;

                    Mensaje<QString> m(UsuarioActivo.getUsuario(),usuamigo,texto,Mensaje<QString>::TEXTO,QDateTime::currentDateTime());
                    GuardarMensajeEnArchivo(m,UsuarioActivo.getUsuario(),usuamigo);

                    //AQUI APLICANDO PILAS
                    pilaDeshacer.insertar(m);
                    MensajeEliminado=false;//aqui no se a eliminado nada aun
                    entradaMensaje->clear();
                    actualizarMensajes();

                });

                connect(btnDeshacer,&QPushButton::clicked,this,[=]()
                {

                    if (pilaDeshacer.empty()) return;

                    //aqui obtiene el mensaje en la cima de la pila
                    Mensaje<QString> m=pilaDeshacer.top();

                    //aqui cargaa los mensajes actuales desde el archivo
                    QString rutaArchivo=ObtenerRutaArchivo(UsuarioActivo.getUsuario(), usuamigo);
                    QFile archivo(rutaArchivo);
                    if(!archivo.open(QIODevice::ReadOnly|QIODevice::Text))return;

                    QStringList lineas;
                    QTextStream in(&archivo);
                    while(!in.atEnd())lineas<<in.readLine();
                    archivo.close();

                    //aqui Formato del mensaje
                    QString formato="["+m.getFecha().toString("yyyy-MM-dd HH:mm")+"] "+m.getEmisor()+" ("+(m.getTipo()==Mensaje<QString>::TEXTO?"texto":"sticker") + "): "+m.getContenido();

                    //aqui verifica si es el ultimo mensaje en el archivo
                    if(!lineas.isEmpty()&&lineas.last()==formato)
                    {

                        //aqui permite deshacer
                        pilaRehacer.insertar(m);
                        lineas.removeLast();

                        if(archivo.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                        {

                            QTextStream out(&archivo);
                            for(const QString &l:lineas)out<<l<<"\n";
                            archivo.close();

                        }

                        MensajeEliminado = true;
                        actualizarMensajes();

                    }else{

                        //si no es el ultimo mensaje, no se puede deshacer
                        QMessageBox::information(this, "Deshacer no permitido","Solo puedes deshacer tu ultimo mensaje si no hay otro despues.");

                    }


                });

                connect(btnRestaurar,&QPushButton::clicked,this,[=]()
                {

                    if(!pilaDeshacer.empty()&&MensajeEliminado)
                    {

                        Mensaje<QString>m=pilaDeshacer.top();
                        GuardarMensajeEnArchivo(m,UsuarioActivo.getUsuario(),usuamigo);
                        pilaDeshacer.insertar(m);// vuelve a la pila de deshacer
                        pilaRehacer.pop();//aqui lo quita de rehacer
                        MensajeEliminado=false;
                        actualizarMensajes();

                    }

                });

                actualizarMensajes();//aqui inicial
                disconnect(TimerMensajes,nullptr,nullptr,nullptr);//aqui limpia conexiones anteriores
                connect(TimerMensajes,&QTimer::timeout,this,actualizarMensajes);

            });



    int contactosInicial=amigos.size();

    QTimer *timerContactos=new QTimer(panel);
    connect(timerContactos,&QTimer::timeout,this,[=]()
            {


                QStringList contactosActuales=gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario());
                if(contactosActuales.size()!=contactosInicial)
                {

                    QWidget *nuevo=crearPanelMensajes();
                    int index=paneles->indexOf(panel);
                    paneles->removeWidget(panel);
                    panel->deleteLater();
                    paneles->insertWidget(index, nuevo);
                    paneles->setCurrentIndex(index);

                }

            });
    timerContactos->start(2000); // cada 3 segundos revisa

    QTimer *timerEstado=new QTimer(panel);
    connect(timerEstado,&QTimer::timeout,this,[=]() {
        for (auto it=mapaPuntosEstado.begin();it!=mapaPuntosEstado.end();++it)
        {

            QString usuario=it.key();
            QLabel *punto=it.value();
            bool conectado=Estaconectado(usuario);
            punto->setStyleSheet("background-color: " + QString(conectado ? "green" : "red") + "; border-radius: 6px;");

        }
    });
    timerEstado->start(3000); // cada 3 segundos actualiza el color del punto

    return panel;
}

QWidget *MainWindow::crearPanelNotificaciones()
{

    QWidget *panel=new QWidget;
    panel->setObjectName("panelbuscar");
    panel->setStyleSheet("background-color: #ecf0f1;");
    QVBoxLayout *layoutPanel=new QVBoxLayout(panel);
    layoutPanel->setContentsMargins(20,20,20,20);
    layoutPanel->setSpacing(15);

    QLabel *titulo=new QLabel("Buzon de notificaciones - "+UsuarioActivo.getCorreo());
    titulo->setStyleSheet("font-size: 22px; font-weight: bold; color: #2f3640;");
    layoutPanel->addWidget(titulo);

    // Scroll para que pueda ver varias notificaciones
    QScrollArea *scroll=new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget *contenedor=new QWidget;
    QVBoxLayout *layoutNotis=new QVBoxLayout(contenedor);
    contenedor->setStyleSheet("background-color: transparent;");

    //aqui se carga las notificaciones
    GestorNotificaciones gestor;

    //gestor.MarcarComoVistaBuzon(UsuarioActivo.getUsuario());

    // Solo marcamos las tipo "respuesta" como vistas al abrir el panel
    QList<Notificacion> notis = gestor.cargar(UsuarioActivo.getUsuario());

    //aqui esta linea es clave para que aparezca bien las solicitudes cuando tenga las 2 pantallas activas
    gestionContactos.CargarDatos();

    for (int i=0; i<notis.size();++i) {
        const Notificacion &n=notis[i];

        QWidget *card=new QWidget;
        card->setStyleSheet("background-color: white; border: 1px solid #dcdde1; border-radius: 10px;");
        QVBoxLayout *layout=new QVBoxLayout(card);
        layout->setContentsMargins(15,10,15,10);

        QLabel *titulo=new QLabel;
        QLabel *mensaje=new QLabel;
        QLabel*fechaHora=new QLabel("🕒 " +n.getFecha()+" "+n.getHora());
        fechaHora->setStyleSheet("font-size: 12px; color: #7f8c8d;");
        titulo->setStyleSheet("font-size: 16px; font-weight: bold; color: #192a56;");
        mensaje->setStyleSheet("font-size: 14px; color: #353b48;");

        if(n.getTipo()=="solicitud")
        {

            titulo->setText("🔔 Solicitud de amistad");

            //aqui estado exacto de esta solicitud
            QString estado=n.getRelacionado();//aqui, es quien eenvio solicitud
            QString EstadoReal=gestionContactos.ObtenerEstadoSolicitud(estado,UsuarioActivo.getUsuario());

            if(EstadoReal=="aceptada")
            {

                mensaje->setText("✅ Solicitud aceptada.");

            }else if(EstadoReal=="rechazada"){

                mensaje->setText("❌ Solicitud rechazada.");

            }else{

                mensaje->setText(estado + " te ha enviado una solicitud de amistad.");

            }

            layout->addWidget(titulo);
            layout->addWidget(mensaje);
            layout->addWidget(fechaHora);

            if(EstadoReal=="pendiente")
            {

                //aqui  Solo si est pendiente mostramos los botones
                QHBoxLayout *botonera=new QHBoxLayout;
                QPushButton *btnAceptar=new QPushButton("Aceptar");
                QPushButton *btnRechazar=new QPushButton("Rechazar");

                btnAceptar->setStyleSheet("background-color: #27ae60; color: white; padding: 5px 12px; border-radius: 5px;");
                btnRechazar->setStyleSheet("background-color: #e74c3c; color: white; padding: 5px 12px; border-radius: 5px;");

                botonera->addWidget(btnAceptar);
                botonera->addWidget(btnRechazar);
                layout->addLayout(botonera);

                connect(btnAceptar, &QPushButton::clicked,this,[=]() mutable {
                    gestionContactos.AceptarSolicitud(n.getRelacionado(), UsuarioActivo.getUsuario());
                    CrearCarpetaConversacion(UsuarioActivo.getUsuario(),n.getRelacionado());

                    Notificacion respuesta(n.getRelacionado(), UsuarioActivo.getUsuario(), "respuesta", UsuarioActivo.getUsuario() + " aceptó tu solicitud.","nueva");
                    GestorNotificaciones().agregarnotificacion(respuesta);
                    GestorNotificaciones().MarcarComoVista(UsuarioActivo.getUsuario(), i);

                    ActualizarContadorNotificaciones();//esto actualiza el contador a 0 si ya no hay nuevas

                    mensaje->setText("✅ Solicitud aceptada.");
                    btnAceptar->hide();
                    btnRechazar->hide();

                    QWidget *viejopanel=this->findChild<QWidget*>("panelBuscar");
                    if(viejopanel)
                    {

                        int indice=paneles->indexOf(viejopanel);
                        if(indice!=-1)
                        {

                            QWidget*nuevo=crearPanelBuscar();
                            paneles->removeWidget(viejopanel);
                            delete viejopanel;
                            paneles->insertWidget(indice, nuevo);
                            if (paneles->currentIndex()==indice)
                            {

                                paneles->setCurrentIndex(indice);

                            }

                        }
                    }
                });

                connect(btnRechazar, &QPushButton::clicked,this,[=]()mutable
                        {

                            if(gestionContactos.RechazarSolicitud(n.getRelacionado(), UsuarioActivo.getUsuario()))
                            {

                                Notificacion respuesta(n.getRelacionado(), UsuarioActivo.getUsuario(), "respuesta", UsuarioActivo.getUsuario() + " rechazó tu solicitud.","nueva");
                                GestorNotificaciones().agregarnotificacion(respuesta);
                                GestorNotificaciones().MarcarComoVista(UsuarioActivo.getUsuario(), i);

                                ActualizarContadorNotificaciones();

                                mensaje->setText("❌ Solicitud rechazada.");
                                btnAceptar->hide();
                                btnRechazar->hide();

                            }

                        });
            }

        }else if(n.getTipo()=="respuesta"){

            titulo->setText("📩 Notificacion de solicitud");
            mensaje->setText(n.getMensaje());

            layout->addWidget(titulo);
            layout->addWidget(mensaje);
            layout->addWidget(fechaHora);

        }

        layoutNotis->addWidget(card);
    }

    scroll->setWidget(contenedor);
    layoutPanel->addWidget(scroll);

    ActualizarContadorNotificaciones();

    //actualizacion con qtimer para que cuando se envia una notificacion se vea en tiempo real
    QTimer *timerNoti=new QTimer(panel);
    connect(timerNoti,&QTimer::timeout,this,[=](){

        if(paneles->currentWidget()==panel)
        {

            QWidget *nuevo=crearPanelNotificaciones();
            int index=paneles->indexOf(panel);
            paneles->removeWidget(panel);
            panel->deleteLater(); // eliminamos el anterior
            paneles->insertWidget(index, nuevo);
            paneles->setCurrentIndex(index);

        }

    });
    timerNoti->start(2000);//2 segunditossss


    //aqui se actualiza el contador despues de marcarlas
    ActualizarContadorNotificaciones();

    return panel;
}

QWidget *MainWindow::crearPanelHistorial()
{

    QWidget *panel = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(panel);

    QLabel *titulo = new QLabel("Historial de Conversaciones");
    titulo->setStyleSheet("font-size: 20px; font-weight: bold; color: #2c3e50;");
    layout->addWidget(titulo);

    // Campo de búsqueda
    this->buscadorHistorial=new QLineEdit;
    buscadorHistorial->setPlaceholderText("Buscar palabra clave...");
    buscadorHistorial->setStyleSheet(
        "padding: 8px; font-size: 14px; color: #2c3e50;"
        "border: 1px solid #2ecc71; border-radius: 5px;");
    layout->addWidget(buscadorHistorial);
    //this->buscadorHistorial = buscador;

    // Combo de ordenamiento
    this->comboOrdenHistorial= new QComboBox;
    comboOrdenHistorial->addItem("Ordenar por fecha");
    comboOrdenHistorial->addItem("Ordenar por orden alfabetico contacto");
    comboOrdenHistorial->addItem("Ordenar por longitud del mensaje");
    comboOrdenHistorial->setStyleSheet(
        "padding: 8px; font-size: 14px; color: #2c3e50;"
        "border: 1px solid #2ecc71; border-radius: 5px;");
    layout->addWidget(comboOrdenHistorial);
    //this->comboOrdenHistorial = ordenCombo;

    // Lista para mostrar resultados
    this->listaHistorialMensajes = new QListWidget;
    listaHistorialMensajes->setMinimumHeight(400);
    listaHistorialMensajes->setStyleSheet(
        "font-size: 13px; color: #34495e;"
        "background-color: #ecf0f1; border: 1px solid #bdc3c7; border-radius: 5px;");
    layout->addWidget(listaHistorialMensajes);
   // this->listaHistorialMensajes = lista;

    // Espaciado final
    layout->addStretch();

    connect(buscadorHistorial, &QLineEdit::textChanged, this, [this](const QString &texto){
        QVector<InfoHistorial> filtrados;

        for (const InfoHistorial &m : historialCompleto) {
            if (m.contenido.contains(texto, Qt::CaseInsensitive) ||
                m.emisor.contains(texto, Qt::CaseInsensitive) ||
                m.contacto.contains(texto, Qt::CaseInsensitive)) {
                filtrados.append(m);
            }
        }

        MostrarMensajesEnLista(filtrados);
    });

    connect(comboOrdenHistorial, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                OrdenarHistorial(index);
            });

    return panel;
}

void MainWindow::ActualizarContadorNotificaciones()
{

    GestorNotificaciones gestor;
    QList<Notificacion>notis=gestor.cargar(UsuarioActivo.getUsuario());
    int pendiente=0;
    for(const Notificacion &n:notis)
    {

        if((n.getTipo()=="solicitud"||n.getTipo()=="respuesta")&& n.getEstado().trimmed()=="nueva")
        {

            pendiente++;

        }

    }
    if(pendiente>0)
    {

        btnNotificaciones->setText("Buzon ("+QString::number(pendiente)+")");

    }else{

        btnNotificaciones->setText("Buzon");

    }

}

void MainWindow::GuardarEstadoMensajes()
{


    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();  // salir de /debug o /release
    QDir carpeta(dir.filePath("estado_mensajes"));
    if(!carpeta.exists())carpeta.mkpath(".");

    QString ruta=carpeta.filePath("estado_" + UsuarioActivo.getUsuario() + ".txt");
    QFile file(ruta);
    if(file.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        QTextStream out(&file);
        for(auto messi=ultimosMensajesContados.begin();messi!=ultimosMensajesContados.end();++messi)
        {

            out<<messi.key()<<";"<<messi.value()<<";"<<colasNoLeidos[messi.key()].size()<<"\n";

        }
        file.close();

    }


}

void MainWindow::CargarEstadoMensajes()
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QDir carpeta(dir.filePath("estado_mensajes"));
    QString ruta = carpeta.filePath("estado_" + UsuarioActivo.getUsuario() + ".txt");

    QFile file(ruta);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&file);
        while(!in.atEnd())
        {

            QString linea=in.readLine();
            QStringList partes=linea.split(';');
            if(partes.size()==3)
            {

                QString usuario=partes[0];
                int mensajesPrevios=partes[1].toInt();
                int cola=partes[2].toInt();
                ultimosMensajesContados[usuario]=mensajesPrevios;

                ColaNoLeidos<Mensaje<QString>> colaTemp;
                for (int i = 0; i < cola; ++i)
                    colaTemp.Insertar(Mensaje<QString>());
                colasNoLeidos[usuario] = colaTemp;

            }

        }
        file.close();

    }

}

void MainWindow::mostrarPanelStickersFavoritos()
{

    //aqui se limpia el panel de stickers si ya existe
    if(panelStickers)
    {

        panelStickers->close();
        delete panelStickers;
        panelStickers=nullptr;

    }

    //aqui se crea el panel flotante
    panelStickers=new QWidget(this);
    panelStickers->setWindowFlags(Qt::Popup);
    panelStickers->setStyleSheet("background-color: white; border: 1px solid gray;");
    panelStickers->setMinimumSize(300,200);

    QVBoxLayout*layout=new QVBoxLayout(panelStickers);
    layout->setContentsMargins(10,10,10,10);

    //aqui titulo con el nombre de usuario
    QLabel *titulo=new QLabel("Mis Stickers - "+UsuarioActivo.getUsuario());
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50; padding-bottom: 10px;");
    layout->addWidget(titulo);


    //aqui layout en forma de grilla para los stickers
    QGridLayout*grid=new QGridLayout;
    grid->setSpacing(10);
    layout->addLayout(grid);

    //aqui se leen los stickers favoritos del archivo del usuario
    QString ruta=obtenerRutaFavoritos(UsuarioActivo.getUsuario());
    QFile archivo(ruta);
    QStringList lista;

    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream entrada(&archivo);
        while(!entrada.atEnd())
        {

            QString linea=entrada.readLine().trimmed();
            if(!linea.isEmpty())
            {

                lista<<linea;

            }

        }
        archivo.close();

    }

    QDir carpeta(obtenerRutaStickersDisponibles());
    int fila=0,columna=0;
    for(const QString &nombre:lista)
    {

        QString nombreNormal=nombre.trimmed();
        if(nombreNormal.isEmpty())continue;
        QString rutaSticker=carpeta.filePath(nombreNormal);
        if(!QFile::exists(rutaSticker))
        {

            qDebug() << "No se encontroooooooooooooooooooooooooo:" << rutaSticker;
            continue;

        }

        ClickableLabel*img=new ClickableLabel;
        img->setPixmap(QPixmap(rutaSticker).scaled(80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        img->setCursor(Qt::PointingHandCursor);
        img->setToolTip(nombre);

        connect(img,&ClickableLabel::clicked,this,[=](){

            EnviarStickers(nombre);
            panelStickers->close();

        });

        grid->addWidget(img,fila,columna);
        columna++;
        if(columna==4)
        {

            columna=0;
            fila++;

        }

    }
    panelStickers->adjustSize();
    QPoint posicion=QCursor::pos();//aqui aparece donde esta el cursor
    panelStickers->move(posicion.x(),posicion.y()-panelStickers->height());

    panelStickers->show();

}

void MainWindow::EnviarStickers(const QString &nombreSticker)
{

    if (chatActivo.isEmpty()) return;

    // Mostrar visualmente el sticker en el chat
    QLabel *stickerLabel = new QLabel;
    QString ruta = obtenerRutaStickersDisponibles() + "/" + nombreSticker;
    stickerLabel->setPixmap(QPixmap(ruta).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    stickerLabel->setStyleSheet("background-color: transparent; border-radius: 10px;");
    stickerLabel->setAlignment(Qt::AlignRight);
    layoutMensajes->addWidget(stickerLabel);

    // Guardar en archivo
    QString rutaArchivo = ObtenerRutaConversacion(UsuarioActivo.getUsuario(), chatActivo);
    QFile archivo(rutaArchivo);
    if (archivo.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&archivo);
        QDateTime ahora = QDateTime::currentDateTime();
        QString fecha = ahora.toString("yyyy-MM-dd");
        QString hora = ahora.toString("HH:mm");

        out << "[" << fecha << " " << hora << "] "
            << UsuarioActivo.getUsuario() << " (sticker): "
            << nombreSticker << "\n";

        archivo.close();
    }

    // Registrar en pila de deshacer
    Mensaje<QString> mensaje;
    mensaje.setEmisor(UsuarioActivo.getUsuario());
    mensaje.setReceptor(chatActivo);
    mensaje.setContenido(nombreSticker);
    mensaje.setTipo(Mensaje<QString>::STICKER);
    mensaje.setFecha(QDateTime::currentDateTime());

    pilaDeshacer.insertar(mensaje);

}

QString MainWindow::ObtenerRutaConversacion(const QString &usuario1, const QString &usuario2) const
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp(); //aqui subimos al directorio raiz del proyecto

    QDir carpeta(dir.filePath("conversaciones"));
    if (!carpeta.exists())
    {

        carpeta.mkpath(".");  //aqui crea carpeta si no existe

    }

    QString u1=usuario1.toLower();
    QString u2=usuario2.toLower();
    if (u1>u2) std::swap(u1, u2);

    QString nombreArchivo=u1+"_"+u2+".txt";
    return carpeta.filePath(nombreArchivo);

}

QString MainWindow::obtenerRutaStickersDisponibles() const
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    return dir.filePath("stickers_disponibles");

}

QString MainWindow::obtenerRutaFavoritos(const QString &usuario) const
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QDir carpeta(dir.filePath("stickers_usuarios"));
    if (!carpeta.exists())
    {

        carpeta.mkpath(".");

    }
    return carpeta.filePath("stickers_" + usuario + ".txt");

}

void MainWindow::GuardarBorradores()
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QDir carpeta(dir.filePath("borradores"));
    if(!carpeta.exists()) carpeta.mkpath(".");

    QString ruta=carpeta.filePath("borradores_"+UsuarioActivo.getUsuario()+".txt");
    QFile archivo(ruta);

    if(archivo.open(QIODevice::WriteOnly|QIODevice::Text))
    {

        QTextStream out(&archivo);
        for(auto borrador=borradoresMensajes.begin();borrador!=borradoresMensajes.end();++borrador)
        {

            out<<borrador.key()<<"|"<<borrador.value().replace("\n","\\n")<<"\n";

        }
        archivo.close();

    }

}

void MainWindow::CargarBorradores()
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QFile archivo(dir.filePath("borradores/borradores_"+UsuarioActivo.getUsuario()+".txt"));
    if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
    {

        QTextStream in(&archivo);
        while(!in.atEnd())
        {
            QString linea=in.readLine();
            QStringList partes=linea.split("|");
            if(partes.size()==2)
            {

                borradoresMensajes[partes[0]]=partes[1].replace("\\n","\n");

            }

        }
        archivo.close();

    }

}

QVector<MainWindow::InfoHistorial> MainWindow::CargarMensajesHistorial() {
    QVector<InfoHistorial> historial;
    QString usuario = UsuarioActivo.getUsuario();

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp(); // Salimos de /debug o /release
    QDir carpeta(dir.filePath("conversaciones"));
    QStringList archivos = carpeta.entryList(QStringList() << "*.txt", QDir::Files);

    for (const QString &archivo : archivos) {
        if (!archivo.contains(usuario, Qt::CaseInsensitive))
            continue;

        QString ruta = carpeta.filePath(archivo);
        QFile file(ruta);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString linea = in.readLine();

            QRegularExpression re(R"(\[(\d{4}-\d{2}-\d{2}) (\d{2}:\d{2})\] ([^ ]+) \(([^)]+)\): (.+))");
            QRegularExpressionMatch match=re.match(linea);

            if (match.hasMatch()) {
                InfoHistorial info;
                info.fecha = match.captured(1);
                info.hora = match.captured(2);
                info.emisor = match.captured(3);
                QString tipo = match.captured(4);
                QString contenido = match.captured(5);
                info.contenido = (tipo == "sticker") ? "[Sticker] " + contenido : contenido;

                // Obtener contacto desde nombre del archivo
                QString nombreArchivo = archivo;
                nombreArchivo.remove(".txt");
                QStringList partes = nombreArchivo.split("_");
                if (partes.size() == 2) {
                    QString u1 = partes[0];
                    QString u2 = partes[1];
                    info.contacto = (u1 == usuario) ? u2 : u1;
                } else {
                    info.contacto = "Desconocido";
                }

                historial.append(info);
            }
        }

        file.close();
    }

    return historial;
}

void MainWindow::MostrarMensajesEnLista(const QVector<InfoHistorial> &mensajes)
{

    listaHistorialMensajes->clear();
    QString contactoAnterior;

    for(const InfoHistorial &m : mensajes)
    {

        // Si cambia el contacto, insertar encabezado y separación
        if(m.contacto!=contactoAnterior)
        {

            if(!contactoAnterior.isEmpty())
            {

                listaHistorialMensajes->addItem(new QListWidgetItem("")); // Espacio visual

            }

            QListWidgetItem *titulo=new QListWidgetItem("📁 Chat con " + m.contacto + ":");
            titulo->setForeground(Qt::darkBlue);
            titulo->setFont(QFont("Segoe UI", 10, QFont::Bold));
            listaHistorialMensajes->addItem(titulo);

            contactoAnterior=m.contacto;
        }

        // Si es sticker, mostrar nombre + fecha igual que un mensaje
        if(m.contenido.startsWith("[Sticker] "))
        {

            QString nombreSticker=m.contenido.mid(QString("[Sticker] ").length());
            QString rutaSticker=obtenerRutaStickersDisponibles() + "/" + nombreSticker;

            if(QFile::exists(rutaSticker))
            {

                // Texto explicativo
                QString texto=QString("%1 envió un sticker [%2 %3]")
                                    .arg(m.emisor)
                                    .arg(m.fecha)
                                    .arg(m.hora);
                QListWidgetItem*infoItem=new QListWidgetItem(texto);
                infoItem->setForeground(Qt::darkGray);
                listaHistorialMensajes->addItem(infoItem);

                // Imagen visual
                QLabel *imagen=new QLabel;
                imagen->setPixmap(QPixmap(rutaSticker).scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                imagen->setToolTip(QString("%1 [%2 %3]").arg(m.emisor, m.fecha, m.hora));

                QListWidgetItem*item=new QListWidgetItem;
                item->setSizeHint(QSize(120, 110));
                listaHistorialMensajes->addItem(item);
                listaHistorialMensajes->setItemWidget(item, imagen);
                continue;
            }
        }

        // Mensaje normal
        QString texto = QString("%1: %2 [%3 %4]")
                            .arg(m.emisor)
                            .arg(m.contenido)
                            .arg(m.fecha)
                            .arg(m.hora);

        QListWidgetItem *item = new QListWidgetItem(texto);
        item->setForeground(QBrush(Qt::black));
        listaHistorialMensajes->addItem(item);
    }
}

void MainWindow::OrdenarHistorial(int indice)
{

    QVector<InfoHistorial> ordenado = historialCompleto;

    switch (indice){
    case 0:{ //por fecha (ordenamiento burbujaaaaaa)

        for(int i=0;i<ordenado.size();++i)
        {

            for(int j=0;j<ordenado.size()-i-1;++j)
            {

                if(ordenado[j].fecha<ordenado[j+1].fecha||(ordenado[j].fecha==ordenado[j+1].fecha&&ordenado[j].hora>ordenado[j+1].hora))
                {

                    std::swap(ordenado[j],ordenado[j+1]);

                }

            }

        }

        break;
    }

    case 1: {//ordenar por nombre (ordenamiento de inserccionn)

        for(int i=1;i<ordenado.size();++i)
        {

            InfoHistorial llave=ordenado[i];
            int j=i-1;

            while(j>=0&&ordenado[j].contacto>llave.contacto)
            {

                ordenado[j+1]=ordenado[j];
                --j;

            }
            ordenado[j+1]=llave;

        }
        break;
    }

    case 2: {//por longitud del mensaje (ordenamiento shell sort)

        int n=ordenado.size();
        for(int salto=n/2;salto>0;salto/=2)
        {

            for(int i=salto;i<n;++i)
            {

                InfoHistorial aux=ordenado[i];
                int j;
                for(j=i;j>=salto&&ordenado[j-salto].contenido.length()>aux.contenido.length();j-=salto)
                {

                    ordenado[j]=ordenado[j-salto];

                }
                ordenado[j]=aux;

            }

        }
        break;

    }

    }

    MostrarMensajesEnLista(ordenado);

}
