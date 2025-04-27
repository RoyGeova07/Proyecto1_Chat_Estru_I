#ifndef INICIO_H
#define INICIO_H

#include <QWidget>

class QPushButton;
class QLabel;

class Inicio : public QWidget
{
    Q_OBJECT  // <-- Este macro DEBE estar si quieres señales y slots

public:
    explicit Inicio(QWidget *parent = nullptr);

private:
    QLabel *lblTitulo;
    QPushButton *btnLogin;
    QPushButton *btnCrearCuenta;
    QPushButton *btnSalir;

    void AbrirLogIn();
    void AbrirRegistro();
};

#endif // INICIO_H
