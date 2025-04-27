#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"usuario.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Usuario usuario,QWidget *parent=nullptr);
    ~MainWindow()=default;

private:

    Usuario UsuarioActivo;

};
#endif // MAINWINDOW_H
