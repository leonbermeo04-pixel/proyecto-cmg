#include "shaderClass.h"

std::string get_file_contents(const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::end);
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);

}    


Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);        const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();


    // ======================= CREAR Y COMPILAR SHADERS =======================

    // ----- Vertex Shader -----
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // Crea shader tipo vértice
    glShaderSource(vertexShader, 1, &vertexSource, nullptr); // Pasa código fuente
    glCompileShader(vertexShader); // Compila shader
    compileErrors(vertexShader, "VERTEX"); 

    // ----- Fragment Shader -----
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Crea shader tipo fragmento
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr); // Pasa código fuente
    glCompileShader(fragmentShader); // Compila shader
    compileErrors(fragmentShader, "FRAGMENT");

    // ======================= CREAR PROGRAMA DE SHADERS =======================

    ID = glCreateProgram(); // Crea programa
    glAttachShader(ID, vertexShader);    // Adjunta vertex shader
    glAttachShader(ID, fragmentShader);  // Adjunta fragment shader
    glLinkProgram(ID);   // Enlaza ambos
    compileErrors(ID, "PROGRAM");


    // Elimina shaders individuales (ya están dentro del programa)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

// Activa el programa shader para su uso en draw calls.
void Shader::Activate()
{
    glUseProgram(ID);
}

// Elimina el programa shader de la GPU.
void Shader::Delete()
{
    glDeleteProgram(ID);
}

// Comprueba errores de compilacin o enlace y escribe en consola en caso de fallo.
void Shader::compileErrors(unsigned int shader, const char* type)
{
    GLint hasCompiled;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog); 
            std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << std::endl;
        }
    }
}
