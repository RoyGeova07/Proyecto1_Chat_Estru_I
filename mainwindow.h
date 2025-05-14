#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QStackedWidget>
#include<QListWidget>
#include<QLabel>
#include<QPushButton>
#include<QLineEdit>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QTextEdit>
#include"usuario.h"
#include"gestioncontactos.h"
#include<QVector>
#include<functional>
#include<QString>
#include"piladeshacer.h"
#include"Mensaje.h"
#include"colanoleidos.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QListWidget;
class QLabel;
class QPushButton;
class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Usuario usuario,QWidget *parent=nullptr);
    ~MainWindow()=default;

    //aqui se crea  una lista para guardar datos de botones fuera de actualizarMensajes
    struct InfoMensaje
    {

        QString fecha,hora,emisor,tipo,contenido;

    };

    void CrearBotonesAccionMensajes(QVBoxLayout *layoutMensajes,const QVector<InfoMensaje> &mensajes,const QString &usuamigo,std::function<void()> actualizarMensajes,PilaDeshacer<Mensaje<QString>>&pilaDeshacer);

    //aqui mapa para las colas de mensajes no leidos por contacto
    QMap<QString,ColaNoLeidos<Mensaje<QString>>> colasNoLeidos;

    //aqui mapa para las etiquetas de contador de mensajes no leidos por contacto
    QMap<QString, QLabel*> etiquetasContador;

    QLabel* contadorMensajesGlobal;
    QMap<QString, int> ultimosMensajesContados;
    QString chatActivo;

    //aqui crea pila para deshacer mensajes enviados
    PilaDeshacer<Mensaje<QString>> pilaDeshacer;
    PilaDeshacer<Mensaje<QString>> pilaRehacer;


private slots:
    void mostrarPanelBuscar();
    void mostrarPanelMensajes();
    void mostrarPanelStickers();
    void mostrarPanelNotificaciones();
    void cerrarSesion();
    void ActualizarContadorNotificaciones();
    void GuardarEstadoMensajes();
    void CargarEstadoMensajes();
    void mostrarPanelStickersFavoritos();
    void EnviarStickers(const QString &nombreSticker);

private:
    Usuario UsuarioActivo;
    GestionContactos gestionContactos;

    QWidget *crearPanelBuscar();
    QWidget *crearPanelMensajes();
    QWidget *crearPanelStickers();
    QWidget *crearPanelNotificaciones();
    QWidget *panelStickers=nullptr;
    QVBoxLayout *layoutMensajes=nullptr;

    // Elementos principales
    QStackedWidget *paneles;
    QPushButton *btnBuscar;
    QPushButton *btnMensajes;
    QPushButton *btnVentanaStickers;
    QPushButton *btnNotificaciones;
    QPushButton *btnCerrarSesion;
    QListWidgetItem* itemContactoSeleccionado = nullptr;

};
#endif // MAINWINDOW_H
