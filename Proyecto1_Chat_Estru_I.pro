QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    funciones_usuario.cpp \
    gestioncontactos.cpp \
    gestornotificaciones.cpp \
    inicio.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    notificacion.cpp \
    registro.cpp \
    solicitudcontacto.cpp \
    usuario.cpp

HEADERS += \
    ClickableLabel.h \
    Mensaje.h \
    colanoleidos.h \
    funciones_usuario.h \
    gestioncontactos.h \
    gestornotificaciones.h \
    inicio.h \
    login.h \
    mainwindow.h \
    notificacion.h \
    piladeshacer.h \
    registro.h \
    solicitudcontacto.h \
    usuario.h

FORMS += \
    login.ui \
    registro.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
