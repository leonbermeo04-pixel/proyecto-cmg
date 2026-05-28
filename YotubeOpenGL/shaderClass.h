#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream> 
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
    GLuint ID;
    // Constructor: carga, compila y enlaza vertex y fragment shaders a un programa.
    Shader(const char* vertexFile, const char* fragmentFile);

    // Activa el programa shader (glUseProgram).
    void Activate();

    // Elimina el programa shader de OpenGL.
    void Delete();
  
private:
    void compileErrors(unsigned int shader, const char* type);
};


#endif
