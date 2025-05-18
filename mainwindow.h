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
#include<QComboBox>

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

    struct InfoHistorial
    {

        QString contacto;
        QString fecha;
        QString hora;
        QString contenido;
        QString emisor;

    };

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
    QMap<QString,QString> borradoresMensajes;

    QTimer *timerChatActivo; // Temporizador para actualizar mensajes en tiempo real
    QMap<QString, QStringList> ultimoMensajeMostrado;

private slots:
    void mostrarPanelBuscar();
    void mostrarPanelMensajes();
    void mostrarPanelStickers();
    void mostrarPanelNotificaciones();
    void mostrarPanelHistorial();
    void cerrarSesion();
    void ActualizarContadorNotificaciones();
    void GuardarEstadoMensajes();
    void CargarEstadoMensajes();
    void mostrarPanelStickersFavoritos();
    void EnviarStickers(const QString &nombreSticker);
    QString obtenerRutaFavoritos(const QString &usuario) const;
    QString ObtenerRutaConversacion(const QString &usuario1, const QString &usuario2) const;
    void GuardarBorradores();
    void CargarBorradores();

private:
    Usuario UsuarioActivo;
    GestionContactos gestionContactos;

    QWidget *crearPanelBuscar();
    QWidget *crearPanelMensajes();
    QWidget *crearPanelStickers();
    QWidget *crearPanelNotificaciones();
    QWidget *crearPanelHistorial();
    QWidget *panelStickers=nullptr;
    QVBoxLayout *layoutMensajes=nullptr;

    QString obtenerRutaStickersDisponibles() const;

    // Elementos principales
    QStackedWidget *paneles;
    QPushButton *btnBuscar;
    QPushButton *btnMensajes;
    QPushButton *btnVentanaStickers;
    QPushButton *btnNotificaciones;
    QPushButton *btnCerrarSesion;
    QPushButton *btnHistorial;
    QLineEdit *buscadorHistorial;
    QComboBox *comboOrdenHistorial;
    QListWidget *listaHistorialMensajes;
    QListWidgetItem* itemContactoSeleccionado = nullptr;
    std::function<void()> actualizarMensajesActual;
    QVector<InfoHistorial> CargarMensajesHistorial();
    void MostrarMensajesEnLista(const QVector<InfoHistorial> &mensajes);
    QVector<InfoHistorial> historialCompleto;
    void OrdenarHistorial(int indice);

};
#endif // MAINWINDOW_H
