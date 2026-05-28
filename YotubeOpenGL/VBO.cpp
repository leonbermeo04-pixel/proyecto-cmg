#include"VBO.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size)
{
    // Genera el buffer y copia los datos de vértices en la GPU.
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::Bind()
{
    // Vincula este VBO como GL_ARRAY_BUFFER.
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
    // Desvincula cualquier GL_ARRAY_BUFFER.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
    // Elimina el buffer de OpenGL.
    glDeleteBuffers(1, &ID);
}