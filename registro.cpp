#include "registro.h"
#include "ui_registro.h"
#include<QMessageBox>
#include<QFileDialog>
#include<QRegularExpression>
#include<QPixmap>
#include<QFile>
#include<QTextStream>
#include<QDir>
#include"usuario.h"
#include"mainwindow.h"
#include"login.h"
#include<QList>
#include"funciones_usuario.h"
#include"inicio.h"

Registro::Registro(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Registro)
{
    ui->setupUi(this);

    ui->btnRegistrar->setEnabled(false);

    //aqui conexiones de señales para la validacionn
    connect(ui->txtUsuario,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->txtNombreCompleto,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->txtContrasena,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->txtConfirmar,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->spnEdad,QOverload<int>::of(&QSpinBox::valueChanged),this,&Registro::ValidarFormulario);
    connect(ui->txtRespuesta,&QLineEdit::textChanged,this,&Registro::ValidarFormulario);
    connect(ui->btnRegistrar,&QPushButton::clicked,this,&Registro::RegistrarUsuario);
    connect(ui->btnCargarAvatar,&QPushButton::clicked,this,&Registro::CargarImagen);
    connect(ui->btnLog,&QPushButton::clicked,this,&Registro::VolverLogIn);
    connect(ui->btnGenerarCorreo,&QPushButton::clicked,this,&Registro::GenerarCorreo);

    connect(ui->txtNombreCompleto,&QLineEdit::textChanged,this,&Registro::HabilitarGenerarCorreo);


}

Registro::~Registro()
{
    delete ui;
}

void Registro::VolverLogIn()
{

    Inicio *log=new Inicio();
    log->show();
    this->close();

}

void Registro::ValidarFormulario()
{

    QString UsuarioIngresado=ui->txtUsuario->text().trimmed();
    QString NombreCompleto=ui->txtNombreCompleto->text().trimmed();
    QString Correo=ui->lblcorreo->text().trimmed();
    QString contrasena=ui->txtContrasena->text();
    QString confirmar=ui->txtConfirmar->text();
    int edad=ui->spnEdad->value();
    QString respuesta=ui->txtRespuesta->text().trimmed();
    //aqui cargo los usuarios
    QList<Usuario> usuarios=CargarUsuarios();
    bool UsuarioUnico=true;
    for(const Usuario &u:usuarios)
    {

        if(u.getUsuario()==UsuarioIngresado)
        {

            UsuarioUnico=false;
            break;

        }

    }
    bool valido=true;

    // setToolTip me sirve para definir o cambiar el texto que se mostrara como informacion sobre herramientas (tooltip)
    //cuando el usuario pasa el cursor del raton por encima de un elemento de la interfaz grafica.

    //aqui validaciones
    if(UsuarioIngresado.isEmpty())
    {

        valido=false;
        ui->txtUsuario->setToolTip("El nombre de usuario no puede estar vacio.");
        ui->txtUsuario->setStyleSheet("background-color: lightcoral;");

    }else if(UsuarioIngresado.contains(" ")){

        valido = false;
        ui->txtUsuario->setToolTip("El nombre de usuario no puede contener espacios.");
        ui->txtUsuario->setStyleSheet("background-color: lightcoral;");

    }else if(UsuarioIngresado.length()<3){

        valido = false;
        ui->txtUsuario->setToolTip("El nombre de usuario debe tener al menos 3 caracteres.");
        ui->txtUsuario->setStyleSheet("background-color: lightcoral;");

    }else if(UsuarioIngresado.compare(NombreCompleto,Qt::CaseInsensitive)==0){

        valido = false;
        ui->txtUsuario->setToolTip("El nombre de usuario no puede ser igual al nombre completo.");
        ui->txtUsuario->setStyleSheet("background-color: lightcoral;");

    }else if(!UsuarioUnico){

        valido=false;
        ui->txtUsuario->setToolTip("El nombre de usario ya existe");
        ui->txtUsuario->setStyleSheet("background-color: lightcoral;");

    }else{

        ui->txtUsuario->setToolTip("");
        ui->txtUsuario->setStyleSheet("background-color: lightgreen;");

    }

    if(NombreCompleto.isEmpty())
    {

        valido=false;
        ui->txtNombreCompleto->setToolTip("El nombre completo no puede estar vacio");
        ui->txtNombreCompleto->setStyleSheet("background-color: lightcoral;");

    }else if(NombreCompleto.contains('@')){

        valido = false;
        ui->txtNombreCompleto->setToolTip("El nombre completo no puede contener el caracter '@'.");
        ui->txtNombreCompleto->setStyleSheet("background-color: lightcoral;");

    }else{

        ui->txtNombreCompleto->setToolTip("");
        ui->txtNombreCompleto->setStyleSheet("background-color: lightgreen;");

    }

    QRegularExpression regexCorreo(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");

    if(Correo.isEmpty()||!regexCorreo.match(Correo).hasMatch())
    {

        valido=false;
        ui->lblcorreo->setToolTip("Debe generar un correo valido, por ejemplo: nombre@gmail.com");
        ui->lblcorreo->setStyleSheet("color: red;");

    }else{

        ui->lblcorreo->setToolTip("");
        ui->lblcorreo->setStyleSheet("color: lightgreen;");

    }

    if(contrasena.length()<6)
    {

        valido=false;
        ui->txtContrasena->setToolTip("La contraseña debe de tener al menos 6 caracteres");
        ui->txtContrasena->setStyleSheet("background-color: lightcoral;");

    }else{

        ui->txtContrasena->setToolTip("");
        ui->txtContrasena->setStyleSheet("background-color: lightgreen;");

    }

    if(contrasena!=confirmar)
    {

        valido=false;
        ui->txtConfirmar->setToolTip("Las constraseñas no coinciden");
        ui->txtConfirmar->setStyleSheet("background-color: lightcoral;;");

    }else{

        ui->txtConfirmar->setToolTip("");
        ui->txtConfirmar->setStyleSheet("background-color: lightgreen;");

    }

    if(edad<12)
    {

        valido=false;
        ui->spnEdad->setToolTip("Debes tener minimo 12 años");


    }else{

        ui->spnEdad->setToolTip("");

    }

    if(respuesta.isEmpty())
    {

        valido=false;
        ui->txtRespuesta->setToolTip("La respuesta de seguridad no puede estar vacia.");
        ui->txtRespuesta->setStyleSheet("background-color: lightcoral;");

    }else{

        ui->txtRespuesta->setToolTip("");
        ui->txtRespuesta->setStyleSheet("background-color: lightgreen;");

    }

    if(Correo.contains('@'))
    {

        ui->btnRegistrar->setEnabled(false);

    }else{

        ui->btnGenerarCorreo->setEnabled(true);

    }

    ui->btnRegistrar->setEnabled(valido);
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

void Registro::GenerarCorreo()
{

    QString nombreCompleto=ui->txtNombreCompleto->text().trimmed();

    if(nombreCompleto.isEmpty())
    {

        //si no hay nada que generar
        QMessageBox::warning(this,"Advertencia","Debe ingresar su nombre completo para generar el correo");

    }

    QString correo=nombreCompleto.replace(" ","").toLower()+"@gmail.com";


    //aqui se verifica si ya existe el correo en el archivo
    QList<Usuario> usuarios=CargarUsuarios();
    for(const Usuario &u:usuarios)
    {

        if(u.getUsuario()==correo)
        {

            QMessageBox::warning(this,"Error","El correo generado ya existe. Modifique el nombre completo para uno unico");
            return;

        }

    }

    //si pasa todo, asignar el correo
    ui->lblcorreo->setText(correo);
    ui->btnGenerarCorreo->setEnabled(false);//se bloqueara hasta que modifique de nuevo el nombre
    ValidarFormulario();

}

void Registro::RegistrarUsuario()
{

    QList<Usuario> usuarios=CargarUsuarios();
    QString UsuarioIngresado=ui->txtUsuario->text().trimmed();

    for(const Usuario &u:usuarios)
    {

        if(u.getUsuario()==UsuarioIngresado)
        {

            QMessageBox::warning(this,"Error","El nombre de usuario ya existe");
            return;

        }

    }

    Usuario nuevo(
        ui->txtUsuario->text(),
        ui->txtNombreCompleto->text(),
        ui->lblcorreo->text(),
        ui->txtContrasena->text(),
        ui->spnEdad->value(),
        ui->cbmPregunta->currentText(),
        ui->txtRespuesta->text(),
        RutaAvatar,
        true  // Al registrarse, el usuario se considera conectado
        );

    QFile archivo(ObtenerRutaUsuarios());
    if(archivo.open(QIODevice::Append|QIODevice::Text))
    {

        QTextStream out(&archivo);
        out<<nuevo.serializar() << "\n";  //aqui usamos la funcion de la clase Usuario
        archivo.close();

        MarcarUsuarioComoConectado(nuevo.getUsuario());

        //aqui se crea la carpeta de stickers si no existe
        QString rutaBase=QDir(QCoreApplication::applicationDirPath()).absolutePath();
        QString CarpetaStickers=rutaBase+"/stickers_usuarios";

        QDir().mkpath(CarpetaStickers);

        //aqui se crea el archivo de stickers personal del usuario
        QString ArchivoSticker=CarpetaStickers+ "/stickers_" + nuevo.getUsuario() + ".txt";
        QFile ArchivoStickerUsuario(ArchivoSticker);
        if(ArchivoStickerUsuario.open(QIODevice::WriteOnly|QIODevice::Text))
        {

            ArchivoStickerUsuario.close();//vacio se crea crick

        }

        QMessageBox::information(this,"Exito","Usuario "+ui->txtUsuario->text()+ " registrado correctamente.");

        MainWindow *main=new MainWindow(nuevo);
        main->show();

        this->close();

    }else{

        QMessageBox::critical(this,"Error","No se pudo guardar el usuario.");

    }
}

void Registro::HabilitarGenerarCorreo()
{

    ui->btnGenerarCorreo->setEnabled(true);//se habilita siempre que se cambie el nombrecompletoooo

}
