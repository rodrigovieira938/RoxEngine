#include "GLShader.hpp"
#include <glad/gl.h>
#include <fstream>
#include <iostream>
namespace RoxEngine::GL {

    std::string get_file_contents(const std::string&  filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return(contents);
        }
        throw(errno);
    }
    void compileErrors(unsigned int shader, const char* type)
    {
        // Stores status of compilation
        GLint hasCompiled;
        // Character array to store error message in
        char infoLog[1024];
        if (std::strcmp(type,"PROGRAM") != 0)
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << "\n";
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << "\n";
            }
        }
    }

    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        const char* vertexSource = vertexSrc.c_str();
	    const char* fragmentSource = fragmentSrc.c_str();
        GLuint vertexShader = 0, fragmentShader = 0;
        mID = glCreateProgram();
        if(!vertexSrc.empty()) {
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexSource, nullptr);
            glCompileShader(vertexShader);
            compileErrors(vertexShader, "VERTEX");
            glAttachShader(mID, vertexShader);
        }
        if(!fragmentSrc.empty()) {
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
            glCompileShader(fragmentShader);
            compileErrors(fragmentShader, "FRAGMENT");
            glAttachShader(mID, fragmentShader);
        }
        glLinkProgram(mID);
        compileErrors(mID, "PROGRAM");

        if (vertexShader != 0)
            glDeleteShader(vertexShader);
        if (fragmentShader != 0)
            glDeleteShader(fragmentShader);
    }
    Shader::~Shader() {
        glDeleteProgram(mID);
    }
}