#include "Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Guarda el tipo de textura (ej. GL_TEXTURE_2D).
	type = texType;

	// Variables para las dimensiones y canales de la imagen.
	int widthImg, heightImg, numColCh;
	// Convierte la imagen al sistema de coordenadas de OpenGL (0,0 en la esquina inferior).
	stbi_set_flip_vertically_on_load(true);
	// Carga la imagen desde disco en memoria.
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Genera el objeto de textura en la GPU.
	glGenTextures(1, &ID);
	// Activa la unidad de textura indicada y la enlaza.
	glActiveTexture(slot);
	glBindTexture(texType, ID); 

	// Parámetros de filtrado (minificación y magnificación).
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Cómo se repite la textura en las coordenadas S y T.
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Carga los datos de la imagen a la textura GPU y genera mipmaps.
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);

	// Libera la memoria de la imagen (ya está en la GPU).
	stbi_image_free(bytes);

	// Desvincula la textura para evitar modificaciones accidentales.
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader shader, const char* uniform, GLuint unit)
{
	// Obtiene la ubicación del uniform en el shader y le asigna el número de unidad.
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// El shader debe estar activo antes de cambiar uniformes.
	shader.Activate();
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	// Vincula la textura actual para uso en draw calls.
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	// Desvincula la textura activa.
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	// Elimina la textura de la GPU.
	glDeleteTextures(1, &ID);
}