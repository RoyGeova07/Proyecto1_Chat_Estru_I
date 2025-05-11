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


private slots:
    void mostrarPanelBuscar();
    void mostrarPanelMensajes();
    void mostrarPanelHistorial();
    void mostrarPanelNotificaciones();
    void cerrarSesion();
    void ActualizarContadorNotificaciones();

private:
    Usuario UsuarioActivo;
    GestionContactos gestionContactos;

    QWidget *crearPanelBuscar();
    QWidget *crearPanelMensajes();
    QWidget *crearPanelHistorial();
    QWidget *crearPanelNotificaciones();

    // Elementos principales
    QStackedWidget *paneles;
    QPushButton *btnBuscar;
    QPushButton *btnMensajes;
    QPushButton *btnHistorial;
    QPushButton *btnNotificaciones;
    QPushButton *btnCerrarSesion;

};
#endif // MAINWINDOW_H
