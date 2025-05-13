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
    btnHistorial=new QPushButton(QIcon(":/icons/historial.png")," Historial",this);
    btnNotificaciones=new QPushButton(QIcon(":/icons/notification.png"),"",this);
    ActualizarContadorNotificaciones();
    btnCerrarSesion=new QPushButton(QIcon(":/icons/cerrar.png")," Cerrar Sesion",this);

    QString EstiloBoton="QPushButton {background-color: transparent; color: white;  font-size: 16px; padding: 12px; text-align: left;}"
                            "QPushButton:hover {background-color: #40739e; }";

    btnBuscar->setStyleSheet(EstiloBoton);
    btnHistorial->setStyleSheet(EstiloBoton);
    btnMensajes->setStyleSheet(EstiloBoton);
    btnNotificaciones->setStyleSheet(EstiloBoton);
    btnCerrarSesion->setStyleSheet(EstiloBoton);

    //Aqui se crean los botones del lado derecho la parte azulita
    layoutLateral->addWidget(lblAvatar);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(lblNombre);
    layoutLateral->addStretch();
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnBuscar);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnHistorial);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnMensajes);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnNotificaciones);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnCerrarSesion);


    CargarEstadoMensajes();

    // ======= aqui se crea el panel derecho ======
    paneles=new QStackedWidget(this);
    paneles->addWidget(crearPanelBuscar());//aqui con el indiice 0
    paneles->addWidget(crearPanelHistorial());//aqui con el indice 1
    paneles->addWidget(crearPanelMensajes());//aqui con el indice 2
    paneles->addWidget(crearPanelNotificaciones());//aqui nuevo indice 3

    for (auto it = colasNoLeidos.begin(); it != colasNoLeidos.end(); ++it) {
        qDebug() << "Usuario:" << it.key() << " Mensajes no leídos:" << it.value().size();
    }

    //aqui se crea el layout principal
    QHBoxLayout *layoutPrincipal=new QHBoxLayout(central);
    layoutPrincipal->addWidget(barraLateral);
    layoutPrincipal->addWidget(paneles);

    // =======CONEXIONES======
    connect(btnBuscar,&QPushButton::clicked,this,&MainWindow::mostrarPanelBuscar);
    connect(btnHistorial,&QPushButton::clicked,this,&MainWindow::mostrarPanelHistorial);
    connect(btnMensajes,&QPushButton::clicked,this,&MainWindow::mostrarPanelMensajes);
    connect(btnNotificaciones,&QPushButton::clicked,this,&MainWindow::mostrarPanelNotificaciones);
    connect(btnCerrarSesion,&QPushButton::clicked,this,&MainWindow::cerrarSesion);


    //aqui se mostrara el panel por defecto
    paneles->setCurrentIndex(0);

    // Timer que detecta nuevos mensajes reales
    QTimer *timerNuevos = new QTimer(paneles);
    timerNuevos->start(3000);
    connect(timerNuevos, &QTimer::timeout, this, [=]() {
        int total = 0;
        QStringList amigos=gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario());
        for (const QString& usuario : amigos) {
            if (usuario == UsuarioActivo.getUsuario()) continue;

            QStringList mensajes = CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), usuario);
            int totalMensajes = mensajes.size();
            int previos = this->ultimosMensajesContados.value(usuario, 0);
            int nuevos = totalMensajes - previos;

            if (nuevos > 0 && usuario != this->chatActivo) {
                // Insertar en cola
                for (int i = 0; i < nuevos; ++i)
                    this->colasNoLeidos[usuario].Insertar(Mensaje<QString>());

                // Actualizar etiqueta del contacto
                if (this->etiquetasContador.contains(usuario)) {
                    this->etiquetasContador[usuario]->setText(QString::number(this->colasNoLeidos[usuario].size()));
                    this->etiquetasContador[usuario]->setVisible(true);
                }
            }

            // Actualizar conteo interno
            this->ultimosMensajesContados[usuario] = totalMensajes;
            total += this->colasNoLeidos[usuario].size();
        }

        // Mostrar mensaje solo si NO estamos en panel de mensajes
        if (total > 0 && paneles->currentIndex() != 2) {
            this->btnMensajes->setText("Mensajes - ¡Nuevos mensajes!");
        } else if (total == 0) {
            this->btnMensajes->setText("Mensajes");
        }
    });

    QTimer *timerContador=new QTimer(this);
    connect(timerContador,&QTimer::timeout,this,[=]()
    {

        ActualizarContadorNotificaciones();

    });
    timerContador->start(3000); // cada 3 segundos

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

void MainWindow::mostrarPanelHistorial()
{

    QWidget*viejo=paneles->widget(1);
    QWidget*nuevo=crearPanelHistorial();
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

void MainWindow::cerrarSesion()
{

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

        if(gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario()).contains(u.getUsuario()))
        {

            btnSolicitud->setText("Amigos");
            btnSolicitud->setEnabled(false);

        }else if(gestionContactos.ObtenerSolicitudesPendientes(u.getUsuario()).contains(UsuarioActivo.getUsuario())){

            btnSolicitud->setText("Solicitud enviada");
            btnSolicitud->setEnabled(false);

        }else{

            btnSolicitud->setText("Agregar");
            btnSolicitud->setEnabled(true);


        }

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
    timer->start(3000); // Actualiza cada 3 segundos

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
    timerBotones->start(3000);

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
    timerUsuariosNuevos->start(3000); // cada 3 segundos revisa si hay nuevos usuarios

    return panel;
}

//========================================HISTORIAl======================================================================
//========================================HISTORIAl======================================================================
//========================================HISTORIAl======================================================================
//========================================HISTORIAl======================================================================
QWidget *MainWindow::crearPanelHistorial()
{

    QWidget *panel=new QWidget();
    panel->setStyleSheet("background-color: white");
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

        if (!this->ultimosMensajesContados.contains(amigo)) {
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

                QLabel *puntoEstado=new QLabel;
                puntoEstado->setFixedSize(12,12);
                puntoEstado->setStyleSheet("border-radius: 6px;");
                bool conectado=Estaconectado(u.getUsuario());
                puntoEstado->setStyleSheet("background-color: " + QString(conectado ? "green" : "red") + "; border-radius: 6px;");

                //aqui nombre completo
                QLabel *nombreLabel=new QLabel(u.getNombre());
                nombreLabel->setStyleSheet("font-size: 14px; color: #2c3e50; font-weight: bold;");

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
                layout->addStretch();


                // Crear y agregar el contador de mensajes no leidos
                QLabel* etiquetaContador = new QLabel;
                etiquetaContador->setStyleSheet("background-color: red; color: white; font-weight: bold; padding: 2px 6px; border-radius: 10px; font-size: 13px;");
                etiquetaContador->setFixedSize(24, 20);
                etiquetaContador->setAlignment(Qt::AlignCenter);

                // Obtener la cantidad de mensajes no leídos cargados desde archivo (colasNoLeidos)
                int cantidad = 0;
                if (this->colasNoLeidos.contains(u.getUsuario())) {
                    cantidad = this->colasNoLeidos[u.getUsuario()].size();
                } else {
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
    TimerMensajes->start(3000);

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

        // Reset de cola y etiqueta visual
        if (this->colasNoLeidos.contains(usuamigo)) {
            this->colasNoLeidos[usuamigo] = ColaNoLeidos<Mensaje<QString>>();
        }
        if (this->etiquetasContador.contains(usuamigo)) {
            this->etiquetasContador[usuamigo]->setVisible(false);
        }

        // Se considera como leídos todos los mensajes actuales
        this->ultimosMensajesContados[usuamigo] = CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), usuamigo).size();
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
        //aqui se limpia la vista anterior
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
        QVBoxLayout *layoutMensajes = new QVBoxLayout(contenedorMensajes);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(contenedorMensajes);
        chatlayout->addWidget(scrollArea);

        QLineEdit *entradaMensaje = new QLineEdit;
        entradaMensaje->setPlaceholderText("Escribe un mensaje...");
        entradaMensaje->setStyleSheet("color: black; font-size: 14px; padding: 6px;");

        QPushButton *btnEnviar = new QPushButton("Enviar");
        btnEnviar->setStyleSheet("background-color: #3498db; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");

        QPushButton *btnSticker = new QPushButton("Stickers");
        btnSticker->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");

        QPushButton *btnDeshacer = new QPushButton("Deshacer ultimo mensaje");
        btnDeshacer->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px;");

        QPushButton *btnRestaurar = new QPushButton("Restaurar ultimo mensaje");
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
            QScrollBar* scrollBar = scrollArea->verticalScrollBar();
            bool EstaAlFinal=(scrollBar->value()==scrollBar->maximum());
            QLayoutItem *child;
            while((child = layoutMensajes->takeAt(0))!=nullptr)
            {

                if(child->widget()) child->widget()->deleteLater();
                delete child;

            }


            QStringList mensajes = CargarMensajeDesdeArchivo(UsuarioActivo.getUsuario(), usuamigo);
            QString fechaAnterior;


            for (const QString &linea : mensajes) {
                QRegularExpression re(R"(\[(\d{4}-\d{2}-\d{2}) (\d{2}:\d{2})\] ([^ ]+) \(([^)]+)\): (.+))");
                QRegularExpressionMatch match=re.match(linea);

                if (match.hasMatch()) {
                    QString fecha = match.captured(1);
                    QString hora = match.captured(2);
                    QString emisor = match.captured(3);
                    QString tipo = match.captured(4);
                    QString contenido = match.captured(5);

                    if (fecha != fechaAnterior)
                    {

                        QLabel *fechaLabel = new QLabel(QDate::fromString(fecha, "yyyy-MM-dd").toString("dd MMMM yyyy"));
                        fechaLabel->setAlignment(Qt::AlignCenter);
                        fechaLabel->setStyleSheet("color: gray; font-size: 12px; margin: 10px;");
                        layoutMensajes->addWidget(fechaLabel);
                        fechaAnterior = fecha;

                    }

                    QLabel *nombreEmisor = new QLabel(emisor);
                    nombreEmisor->setStyleSheet("font-size: 10px; color: gray;");
                    nombreEmisor->setAlignment(emisor==UsuarioActivo.getNombre()? Qt::AlignRight:Qt::AlignLeft);

                    QLabel *msg = new QLabel(contenido + "\n" + hora);
                    msg->setStyleSheet("padding: 8px 6px; border-radius: 12px; font-size: 14px; font-weight: bold;"); // verde suave + texto oscuro
                    QHBoxLayout *lineaLayout = new QHBoxLayout;

                    if(emisor==UsuarioActivo.getUsuario())
                    {

                        msg->setStyleSheet(msg->styleSheet() + "background-color: #a3e4d7; color: #1c2833;");// gris + texto oscuro
                        msg->setAlignment(Qt::AlignRight);
                        lineaLayout->addWidget(nombreEmisor);
                        lineaLayout->addWidget(msg);
                        lineaLayout->setAlignment(Qt::AlignRight);

                        botonesPendientes.append({fecha, hora, emisor, tipo, contenido});
                    }else{

                        msg->setStyleSheet(msg->styleSheet() + "background-color: #e1e1e1; color: #2d3436;");
                        msg->setAlignment(Qt::AlignLeft);
                        lineaLayout->addWidget(nombreEmisor);
                        lineaLayout->addWidget(msg);
                        lineaLayout->setAlignment(Qt::AlignLeft);

                    }

                    QWidget *lineaWidget = new QWidget;
                    lineaWidget->setLayout(lineaLayout);
                    layoutMensajes->addWidget(lineaWidget);
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

            Mensaje<QString> m(UsuarioActivo.getUsuario(), usuamigo, texto, Mensaje<QString>::TEXTO, QDateTime::currentDateTime());
            GuardarMensajeEnArchivo(m, UsuarioActivo.getUsuario(), usuamigo);

            //AQUI APLICANDO PILAS
            pilaDeshacer.insertar(m);
            MensajeEliminado=false;//aqui no se a eliminado nada aun
            entradaMensaje->clear();
            actualizarMensajes();

        });

        connect(btnDeshacer,&QPushButton::clicked,this,[=]()
        {

            if(!pilaDeshacer.empty())
            {

                Mensaje<QString>m=pilaDeshacer.top();
                pilaRehacer.insertar(m);
                QString rutaArchivo=ObtenerRutaArchivo(UsuarioActivo.getUsuario(),usuamigo);

                QFile archivo(rutaArchivo);
                if(archivo.open(QIODevice::ReadOnly|QIODevice::Text))
                {

                    QStringList lineas;
                    QTextStream in(&archivo);
                    while(!in.atEnd()) lineas<<in.readLine();
                    archivo.close();

                    QString formato="["+m.getFecha().toString("yyyy-MM-dd HH:mm")+"] "+m.getEmisor()+" ("+(m.getTipo()==Mensaje<QString>::TEXTO?"texto":"sticker")+"): "+m.getContenido();
                    lineas.removeAll(formato);

                    if(archivo.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                    {

                        QTextStream out(&archivo);
                        for(const QString &l:lineas)out<<l<<"\n";
                        archivo.close();
                        MensajeEliminado=true;
                        //pilaDeshacer.pop();
                        actualizarMensajes();

                    }

                }

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
    timerContactos->start(3000); // cada 3 segundos revisa

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

            if(EstadoReal=="pendiente"){
                // Solo si está pendiente mostramos los botones
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
    timerNoti->start(3000);//3 segunditossss


    //aqui se actualiza el contador despues de marcarlas
    ActualizarContadorNotificaciones();

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
