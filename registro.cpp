#include "registro.h"
#include "ui_registro.h"
#include<QMessageBox>
#include<QFileDialog>
#include<QRegularExpression>
#include<QPixmap>
#include<QFile>
#include<QTextStream>
#include"usuario.h"

Registro::Registro(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Registro)
{
    ui->setupUi(this);

    ui->btnRegistrar->setEnabled(false);

    //aqui conexiones de señales para la validacionn
    connect(ui->txtUsuario,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->txtCorreo,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->txtContrasena,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->txtConfirmar,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->spnEdad,QOverload<int>::of(&QSpinBox::valueChanged),this,&Registro::ValidarFormulario);
    connect(ui->txtRespuesta,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);

    connect(ui->btnRegistrar,&QPushButton::clicked,this,&Registro::ValidarFormulario);
    connect(ui->btnCargarAvatar,&QPushButton::clicked,this,&Registro::CargarImagen);


}

Registro::~Registro()
{
    delete ui;
}

void Registro::ValidarFormulario()
{

    bool Valido=!ui->txtUsuario->text().isEmpty()&&!ui->txtCorreo->text().isEmpty()&&
    ui->txtCorreo->text().contains(QRegularExpression("@"))&&ui->txtContrasena->text().length()>=6&&
    ui->txtContrasena->text()==ui->txtConfirmar->text()&&ui->spnEdad->value()>=12&&
    !ui->txtRespuesta->text().isEmpty();

    ui->btnRegistrar->setEnabled(Valido);

}

void Registro::CargarImagen()
{

    QString path=QFileDialog::getOpenFileName(this,"Seleccione una imagen","","Imagenes (*.png *.jpg *.jpeg)");
    if(!path.isEmpty())
    {

        QPixmap avatar(path);
        ui->lblAvatar->setPixmap(avatar.scaled(80,80,Qt::KeepAspectRatio));
        RutaAvatar=path;

    }

}

void Registro::RegistrarUsuario()
{

    Usuario nuevo(
        ui->txtUsuario->text(),
        ui->txtNombreCompleto->text(),
        ui->txtCorreo->text(),
        ui->txtContrasena->text(),
        ui->spnEdad->value(),
        ui->cbmPregunta->currentText(),
        ui->txtRespuesta->text(),
        RutaAvatar,
        true  // Al registrarse, el usuario se considera conectado
        );

    QFile archivo("usuarios.txt");
    if(archivo.open(QIODevice::Append|QIODevice::Text))
    {

        QTextStream out(&archivo);
        out<<nuevo.serializar() << "\n";  //aqui usamos la funcion de la clase Usuario
        archivo.close();

        QMessageBox::information(this,"Exito","Usuario registrado correctamente.");
        this->close();

    }
    else
    {

        QMessageBox::critical(this,"Error","No se pudo guardar el usuario.");

    }
}
