#ifndef REGISTRO_H
#define REGISTRO_H

#include <QWidget>

namespace Ui {
class Registro;
}

class Registro : public QWidget
{
    Q_OBJECT

public:
    explicit Registro(QWidget *parent = nullptr);
    ~Registro();

public slots:

    void ValidarFormulario();
    void CargarImagen();
    void RegistrarUsuario();

private:
    Ui::Registro *ui;
    QString RutaAvatar;
};

#endif // REGISTRO_H
