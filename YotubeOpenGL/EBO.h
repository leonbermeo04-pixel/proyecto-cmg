#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>

class EBO
{
public:
    GLuint ID;
    EBO(GLuint* indices, GLsizeiptr size);
    // Crea un EBO (Element Buffer Object) con los índices indicados.
    // indices: puntero a los índices; size: tamaño en bytes.

    // Vincula el EBO como GL_ELEMENT_ARRAY_BUFFER.
    void Bind();

    // Desvincula el EBO (pone 0).
    void Unbind();

    // Elimina el EBO de OpenGL.
    void Delete();
};

#endif 
