#include"VAO.h"

VAO::VAO()
{
    glGenVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
    // Vincula el VBO y configura el puntero de atributo para este VAO.
    VBO.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    // Habilita el atributo para que el shader pueda leerlo.
    glEnableVertexAttribArray(layout);
    VBO.Unbind();
}

void VAO::Bind()
{
    // Bind del VAO actual.
    glBindVertexArray(ID);
}

void VAO::Unbind()
{
    // Unbind: deja sin VAO activo.
    glBindVertexArray(0);
}

void VAO::Delete()
{
    // Elimina el VAO de OpenGL.
    glDeleteVertexArrays(1, &ID);
}