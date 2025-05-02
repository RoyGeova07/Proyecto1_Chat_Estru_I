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

private slots:
    void mostrarPanelBuscar();
    void mostrarPanelMensajes();
    void mostrarPanelContactos();
    void cerrarSesion();

private:
    Usuario UsuarioActivo;
    GestionContactos gestionContactos;

    QWidget *crearPanelBuscar();
    QWidget *crearPanelMensajes();
    QWidget *crearPanelContactos();

    // Elementos principales
    QStackedWidget *paneles;
    QPushButton *btnBuscar;
    QPushButton *btnMensajes;
    QPushButton *btnContactos;
    QPushButton *btnCerrarSesion;

};
#endif // MAINWINDOW_H
