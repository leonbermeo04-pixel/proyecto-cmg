#include"EBO.h"

EBO::EBO(GLuint* indices, GLsizeiptr size)
{
    // Genera el buffer de elementos y copia los índices en la GPU.
    glGenBuffers(1, &ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void EBO::Bind()
{
    // Vincula este EBO para que glDrawElements use sus índices.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}
 
void EBO::Unbind()
{
    // Desvincula cualquier GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
    // Elimina el EBO de OpenGL.
    glDeleteBuffers(1, &ID);
}