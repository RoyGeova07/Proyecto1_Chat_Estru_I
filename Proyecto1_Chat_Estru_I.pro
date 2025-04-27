QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    funciones_usuario.cpp \
    inicio.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    registro.cpp \
    usuario.cpp

HEADERS += \
    funciones_usuario.h \
    inicio.h \
    login.h \
    mainwindow.h \
    registro.h \
    usuario.h

FORMS += \
    login.ui \
    registro.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
