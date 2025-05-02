#ifndef FUNCIONES_USUARIO_H
#define FUNCIONES_USUARIO_H

#include "usuario.h"
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QSet>

QList<Usuario> CargarUsuarios(); // funcion libre, sin clase
void GuardarUsuarios(const QList<Usuario>&usuarios);


#endif // FUNCIONES_USUARIO_H
