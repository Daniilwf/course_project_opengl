#include "SkyboxModel.h"
#include "func.h"
//Подключение stb_image для загрузки текстур
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SkyboxModel::SkyboxModel() {
    // Инициализация переменных
    glGenVertexArrays(1, &vao);
}

void SkyboxModel::loadShaders(const char* vertexShaderPath, const char* fragmentShaderPath) {
    // Переменные под результат компиляции программы
    GLint result = GL_FALSE;
    int infoLogLength;

    // Создание шейдерной программы
    shader_programme = glCreateProgram();

    // Загрузка текстов шейдеров из файлов
    string vstext = LoadShader(vertexShaderPath);
    const char* vertex_shader = vstext.c_str();
    string fstext = LoadShader(fragmentShaderPath);
    const char* fragment_shader = fstext.c_str();

    // Создание вершинного шейдера
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    // Проверка результата компиляции
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);
    // Вывод сообщения об ошибке, если что-то пошло не так
    if (infoLogLength > 0) {
        char* errorMessage = new char[infoLogLength + 1];
        glGetShaderInfoLog(vs, infoLogLength, NULL, errorMessage);
        std::cout << errorMessage;
        delete errorMessage;
    }

    // Аналогично с фрагментным шейдером
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        char* errorMessage = new char[infoLogLength + 1];
        glGetShaderInfoLog(fs, infoLogLength, NULL, errorMessage);
        std::cout << errorMessage;
        delete errorMessage;
    }

    // Сборка программы
    glAttachShader(shader_programme, vs);
    glAttachShader(shader_programme, fs);
    glLinkProgram(shader_programme);
}

void SkyboxModel::loadTexture(const std::vector<std::string>& faces) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); ++i) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SkyboxModel::render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {
    glDepthFunc(GL_LEQUAL);  // Установка глубины до "меньше или равно" для рендеринга скайбокса
    glUseProgram(shader_programme);
    glBindVertexArray(vao);

    // Установка uniform-переменных матриц в шейдерную программу
    GLuint viewLoc = glGetUniformLocation(shader_programme, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    GLuint projectionLoc = glGetUniformLocation(shader_programme, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Рендеринг скайбокса
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);  // Восстановление глубины по умолчанию
}
