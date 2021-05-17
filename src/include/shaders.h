#ifndef SHADERS_H
#define SHADERS_H
#include <stdbool.h>
#include <SDL.h>
#include <gl/glew.h>
#include <SDL_opengl.h>

unsigned int createGlProgram(const GLchar* vertexShaderCode, const GLchar* fragmentShaderCode) {
    int success;
    char infolog[512];

    // compile vertex shader
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderCode, NULL);
    glCompileShader(vs);

    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vs, 512, NULL, infolog);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GL vertex shader compile error\n%s\n", infolog);
        return false;
    }

    // compile fragment shader
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderCode, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fs, 512, NULL, infolog);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GL fragment shader compile error\n%s\n", infolog);
        return false;
    }

    // create program
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if(!success){
        glGetProgramInfoLog(program, 512, NULL, infolog);
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "GL program link error\n%s\n", infolog);
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


#endif