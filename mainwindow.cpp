#include "mainwindow.h"
#include "inicio.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QStackedWidget>
#include <QScrollArea>
#include <QIcon>

MainWindow::MainWindow(Usuario usuario,QWidget *parent)
    : QMainWindow(parent)
    , UsuarioActivo(usuario)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    resize(1000, 600);
    setWindowTitle("Proyecto Chat - Usuario");

    // Configurar fondo blanco
    central->setStyleSheet("background-color: #ecf0f1;");

    // Crear barra lateral
    QWidget *barraLateral = new QWidget(this);
    barraLateral->setFixedWidth(200);
    barraLateral->setStyleSheet("background-color: #2C3E50;");

    QVBoxLayout *layoutLateral = new QVBoxLayout(barraLateral);

    QLabel *lblAvatar = new QLabel(this);
    QPixmap avatar(UsuarioActivo.getRutaAvatar());
    lblAvatar->setPixmap(avatar.scaled(80, 80, Qt::KeepAspectRatio));
    lblAvatar->setFixedSize(80, 80);
    lblAvatar->setAlignment(Qt::AlignCenter);

    QLabel *lblNombre = new QLabel(UsuarioActivo.getNombre(), this);
    lblNombre->setAlignment(Qt::AlignCenter);
    lblNombre->setStyleSheet("color: white; font-weight: bold; font-size: 16px;");

    QPushButton *btnChat = new QPushButton("Chats", this);
    QPushButton *btnContactos = new QPushButton("Contactos", this);
    QPushButton *btnAgregar = new QPushButton("Agregar Contacto", this);
    QPushButton *btnCerrar = new QPushButton("Cerrar Sesión", this);

    QString estiloBoton = "QPushButton { background-color: transparent; color: white; font-size: 16px; padding: 10px; text-align: left;}"
                          "QPushButton:hover { background-color: #34495e; }";

    btnChat->setStyleSheet(estiloBoton);
    btnContactos->setStyleSheet(estiloBoton);
    btnAgregar->setStyleSheet(estiloBoton);
    btnCerrar->setStyleSheet(estiloBoton);

    layoutLateral->addWidget(lblAvatar);
    layoutLateral->addWidget(lblNombre);
    layoutLateral->addSpacing(20);
    layoutLateral->addWidget(btnChat);
    layoutLateral->addWidget(btnContactos);
    layoutLateral->addWidget(btnAgregar);
    layoutLateral->addStretch();
    layoutLateral->addWidget(btnCerrar);

    // Lista de contactos
    listaContactos = new QListWidget(this);
    listaContactos->setStyleSheet("background-color: white; font-size: 14px;");

    // Zona de conversación
    zonaConversacion = new QTextEdit(this);
    zonaConversacion->setReadOnly(true);
    zonaConversacion->setStyleSheet("background-color: white; font-size: 14px;");

    // Layout principal
    QHBoxLayout *layoutPrincipal = new QHBoxLayout(central);
    QVBoxLayout *layoutCentro = new QVBoxLayout();

    layoutCentro->addWidget(listaContactos, 2);  // 2/3 espacio
    layoutCentro->addWidget(zonaConversacion, 4); // 4/3 espacio

    layoutPrincipal->addWidget(barraLateral);
    layoutPrincipal->addLayout(layoutCentro);

    // Conexiones reales
    connect(btnAgregar, &QPushButton::clicked, this, &MainWindow::enviarSolicitud);
    connect(btnContactos, &QPushButton::clicked, this, &MainWindow::verContactos);
    connect(btnChat, &QPushButton::clicked, this, &MainWindow::verSolicitudes);
    connect(btnCerrar, &QPushButton::clicked, this, &MainWindow::cerrarSesion);

    // Cargar lista de contactos inicial
    verContactos();
}

void MainWindow::enviarSolicitud()
{
    QString destinatario = QInputDialog::getText(this, "Enviar Solicitud", "Usuario a agregar:");
    if (!destinatario.isEmpty()) {
        if (gestionContactos.EnviarSolicitud(UsuarioActivo.getUsuario(), destinatario)) {
            QMessageBox::information(this, "Éxito", "Solicitud enviada a " + destinatario);
        } else {
            QMessageBox::warning(this, "Error", "No se pudo enviar la solicitud. Ya existe o eres tú mismo.");
        }
    }
}

void MainWindow::verSolicitudes()
{
    QStringList solicitudes = gestionContactos.ObtenerSolicitudesPendientes(UsuarioActivo.getUsuario());
    if (solicitudes.isEmpty()) {
        QMessageBox::information(this, "Solicitudes", "No tienes solicitudes pendientes.");
    } else {
        QMessageBox::information(this, "Solicitudes Pendientes", solicitudes.join("\n"));
    }
}

void MainWindow::aceptarSolicitud()
{
    QString remitente = QInputDialog::getText(this, "Aceptar Solicitud", "Usuario que te envió la solicitud:");
    if (!remitente.isEmpty()) {
        if (gestionContactos.AceptarSolicitud(remitente, UsuarioActivo.getUsuario())) {
            QMessageBox::information(this, "Éxito", "Solicitud aceptada.");
        } else {
            QMessageBox::warning(this, "Error", "No existe esa solicitud pendiente.");
        }
    }
}

void MainWindow::rechazarSolicitud()
{
    QString remitente = QInputDialog::getText(this, "Rechazar Solicitud", "Usuario que te envió la solicitud:");
    if (!remitente.isEmpty()) {
        if (gestionContactos.RechazarSolicitud(remitente, UsuarioActivo.getUsuario())) {
            QMessageBox::information(this, "Éxito", "Solicitud rechazada.");
        } else {
            QMessageBox::warning(this, "Error", "No existe esa solicitud pendiente.");
        }
    }
}

void MainWindow::verContactos()
{
    listaContactos->clear();
    QStringList contactos = gestionContactos.ObtenerContactos(UsuarioActivo.getUsuario());
    if (!contactos.isEmpty()) {
        for (const QString &contacto : contactos) {
            listaContactos->addItem(contacto);
        }
    }
}

void MainWindow::eliminarContacto()
{
    QString contacto = QInputDialog::getText(this, "Eliminar Contacto", "Usuario a eliminar:");
    if (!contacto.isEmpty()) {
        if (gestionContactos.EliminarContacto(UsuarioActivo.getUsuario(), contacto)) {
            QMessageBox::information(this, "Éxito", "Contacto eliminado.");
            verContactos();
        } else {
            QMessageBox::warning(this, "Error", "No se encontró ese contacto.");
        }
    }
}

void MainWindow::cerrarSesion()
{
    Inicio *inicio = new Inicio();
    inicio->show();
    this->close();
}
