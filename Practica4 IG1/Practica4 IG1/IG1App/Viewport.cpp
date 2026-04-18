#include "Viewport.h"

// Cambia el tamaño del viewport (se llama al redimensionar la ventana).
void Viewport::setSize(GLsizei xw, GLsizei yh)
{
	xWidth = xw;
	yHeight = yh;
}

// Cambia la posición de la esquina inferior izquierda del viewport.
void Viewport::setPos(GLsizei xl, GLsizei yb)
{
	xLeft = xl;
	yBot = yb;
}

// Define el area de dibujo de la ventana en pixeles
void Viewport::upload() const
{
	glViewport(xLeft, yBot, xWidth, yHeight);
}