#pragma once
#include "Model.h"
#include <glm/gtc/type_ptr.hpp>

class SkyboxModel: Model {
private:
    GLuint vao;
    GLuint texture;
    GLuint shader_programme;

public:
    SkyboxModel();


    void loadShaders(const char* vertexShaderPath, const char* fragmentShaderPath);

    void loadTexture(const std::vector<std::string>& faces);

    void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);