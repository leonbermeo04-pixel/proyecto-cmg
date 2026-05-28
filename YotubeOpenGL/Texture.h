#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>

#include "stb_image.h"
#include "shaderClass.h"

class Texture
{
public:
	GLuint ID;
	GLenum type;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
	// Constructor que carga la imagen, crea la textura en GPU y configura parámetros.
	// image: ruta del fichero; texType: GL_TEXTURE_2D; slot: GL_TEXTURE0 + n; format/pixelType: formato de la imagen.

	// Asigna la unidad de textura en el shader para esta textura.
	void texUnit(Shader shader, const char* uniform, GLuint unit);

	// Vincula la textura activa (glBindTexture(type, ID)).
	void Bind();

	// Desvincula la textura activa (glBindTexture(type, 0)).
	void Unbind();

	// Elimina la textura de OpenGL.
	void Delete();
};

#endif