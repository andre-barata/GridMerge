#include <SDL.h>
#include <gl/glew.h>
#include <SDL_opengl.h>
#include <cglm/cglm.h>
#include "include/common.h"
#include "include/res.h"
#include "include/shaders.h"

#include <stdbool.h>
#define CODE(...) #__VA_ARGS__

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight;

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if(!initGlWindow(&windowWidth, &windowHeight)) return terminate();
    if(!initGlew()) return terminate();

    glViewport(0, 0, windowWidth, windowHeight);
    
    float size = 0.5f;
    float vertices[] = {
        -size, -size, -size,
        -size, size, -size,
        size, size, -size,
        size, size, -size,
        size, -size, -size,
        -size, -size, -size,

        -size, size, size,
        -size, size, size,
        size, size, size,
        size, size, size,
        size, -size, size,
        -size, -size, size,
    };
    
    // create a vertex array object
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create a vertex buffer object
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // add the vertex data to the vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), NULL);
    glEnableVertexAttribArray(0);

    mat4 mvpMatrix;
    {
        // make MVP - implements glm::perspective(fov, aspect, zNear, zFar) * glm::lookAt(pos, pos + forward, up);
        vec3 positionVector = {0.0f, 0.0f, -1.0f};
        vec3 forwardVector = {0.0f, 0.0f, 0.1f};
        vec3 projectionVector;
        glm_vec3_add(positionVector, forwardVector, projectionVector);
        mat4 viewDirectionMatrix;
        glm_lookat(positionVector, projectionVector, (vec3){0.0f, 1.0f, 0.0f}, viewDirectionMatrix);
        mat4 perspectiveMatrix;
        glm_perspective(/*fov*/70.0f, /*aspectRatio*/(float)windowWidth/(float)windowHeight, /*near*/0.1f, /*far*/100.0f, perspectiveMatrix);
        glm_mul(perspectiveMatrix, viewDirectionMatrix, mvpMatrix);
    }

    // Compile Shaders and set Gl pipeline
    const GLchar* vertexShaderCode = "#version 330 core\n" CODE(
        layout (location = 0) in vec3 pos;
        uniform mat4 mvp;
        void main() {
            gl_Position = mvp * vec4(pos, 1.0);
        }
    );
    const GLchar* fragmentShaderCode = "#version 330 core\n" CODE(
        out vec4 color;
        void main() {
            color = vec4(0.4f, 0.4f, 0.4f, 0.3f);
        } 
    );
    unsigned int program = createGlProgram(vertexShaderCode, fragmentShaderCode);
    if (program == false) return terminate();


    // get uniforms
    GLuint uniforms[1] = { glGetUniformLocation(program, "mvp") };


    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // start render loop
    bool quit = false;
    while(!quit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) quit = true;
            if(event.type == SDL_QUIT) quit = true;
        }
        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vao);

        static float rot; rot += 0.01f;

        mat4 transformMatrix;
        {
            // make transform
            mat4 positionMatrix, scaleMatrix, xRotaionMatrix, yRotaionMatrix, zRotaionMatrix, rotaionMatrix;
            glm_translate_make(positionMatrix, (vec3){0.0f, 0.0f, 0.0f});
            glm_scale_make(scaleMatrix, (vec3){1.0f, 1.0f, 1.0f});
            glm_rotate_make(xRotaionMatrix, rot, (vec3){1.0f, 0.0f, 0.0f});
            glm_rotate_make(yRotaionMatrix, rot, (vec3){0.0f, 1.0f, 0.0f});
            glm_rotate_make(zRotaionMatrix, rot, (vec3){0.0f, 0.0f, 1.0f});
            glm_mat4_mulN((mat4 *[]){&xRotaionMatrix, &yRotaionMatrix, &zRotaionMatrix}, 3, rotaionMatrix);
            glm_mat4_mulN((mat4 *[]){&positionMatrix, &rotaionMatrix, &scaleMatrix}, 3, transformMatrix);
        }
        
        glm_mul(transformMatrix, mvpMatrix, transformMatrix);
        // assign transform uniform
        glUniformMatrix4fv(uniforms[0], 1, GL_FALSE, &transformMatrix[0][0]);
/*
    glm::mat4 posMat = glm::translate(pos);
    glm::mat4 scaleMat = glm::scale(scale);
    glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 rotMat = rotX * rotY * rotZ;
/*
glm_translate(m, pivot);
glm_rotate(m, angle, axis);
glm_translate(m, pivotInv); /* pivotInv = -pivot
*/



        glDrawArrays(GL_TRIANGLES, 0, 12);
        SDL_GL_SwapWindow(mainWindow);
    }
    glDeleteProgram(program);
    return terminate();
}
