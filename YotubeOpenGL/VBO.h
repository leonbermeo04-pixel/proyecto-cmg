#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO
{
public:
    GLuint ID;
    VBO(GLfloat* vertices, GLsizeiptr size);

    // Crea un VBO con los datos de vértices indicados.
    // vertices: puntero a los datos; size: tamaño en bytes.

    // Vincula el buffer como GL_ARRAY_BUFFER.
    void Bind();

    // Desvincula el buffer (pone 0).
    void Unbind();

    // Elimina el buffer de OpenGL.
    void Delete();
};

#endif
