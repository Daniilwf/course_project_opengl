#include "Model.h"
#include "func.h"
#include "globals.h"
//Подключение stb_image для загрузки текстур
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Model::render(GLuint mode)
{
    //Самый простой вариант - просто рисует, ничего в шейдер не передаёт.
    //Выбираем шейдерную программу
    glUseProgram(shader_programme);
    //Активируем массив вершин
    glBindVertexArray(vao);
    //Если индексы есть - рисуем по ним
    if (indices_count > 0)
        glDrawElements(mode, indices_count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(mode, 0, verteces_count);
}

void Model::render(glm::mat4 MVPMatrix, GLuint mode)
{
    //Более сложный вариант - с использованием матрицы преобразований

    //Сначала всегда выбираем программу
    glUseProgram(shader_programme);

    //Теперь надо поискать в ней матрицу
    GLuint MVP = glGetUniformLocation(shader_programme, "MVP");
    //Как нашли - передаём в неё значения
    glUniformMatrix4fv(MVP, 1, GL_FALSE, &MVPMatrix[0][0]);

    //А дальше всё так же как и в простом случае
    glBindVertexArray(vao);
    if (indices_count > 0)
        glDrawElements(mode, indices_count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(mode, 0, verteces_count);

}

void Model::render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix, GLuint mode)
{
    //А теперь три матрицы

    //Сначала всегда выбираем программу
    glUseProgram(shader_programme);

    GLuint Matrix = glGetUniformLocation(shader_programme, "M");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &modelMatrix[0][0]);
    Matrix = glGetUniformLocation(shader_programme, "V");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &viewMatrix[0][0]);
    Matrix = glGetUniformLocation(shader_programme, "P");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &projMatrix[0][0]);

    //А дальше всё так же как и в простом случае
    glBindVertexArray(vao);
    if (indices_count > 0)
        glDrawElements(mode, indices_count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(mode, 0, verteces_count);
}

void Model::render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix, LightsInfo lights, GLuint mode)
{
    //А теперь ещё и освещение!

    //Сначала всегда выбираем программу
    glUseProgram(shader_programme);

    //Теперь матрицы
    GLuint Matrix = glGetUniformLocation(shader_programme, "M");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &modelMatrix[0][0]);
    Matrix = glGetUniformLocation(shader_programme, "V");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &viewMatrix[0][0]);
    Matrix = glGetUniformLocation(shader_programme, "P");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &projMatrix[0][0]);

    //И свет... это долго
    //Сначала фоновый
    GLuint la = glGetUniformLocation(shader_programme, "lights.dirLight.ambient");
    glUniform3fv(la, 1, &lights.dirLight.ambient[0]);
    GLuint ld = glGetUniformLocation(shader_programme, "lights.dirLight.diffuse");
    glUniform3fv(ld, 1, &lights.dirLight.diffuse[0]);
    GLuint ls = glGetUniformLocation(shader_programme, "lights.dirLight.specular");
    glUniform3fv(ls, 1, &lights.dirLight.specular[0]);
    GLuint ldir = glGetUniformLocation(shader_programme, "lights.dirLight.direction");
    glUniform3fv(ldir, 1, &lights.dirLight.direction[0]);

    //Теперь точеные
    GLuint lnp = glGetUniformLocation(shader_programme, "lights.numPLights");
    glUniform1i(lnp, lights.numPLights);
    for (int i = 0; i < lights.numPLights; i++)
    {
        GLuint lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].ambient").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].ambient[0]);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].diffuse").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].diffuse[0]);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].specular").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].specular[0]);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].position").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].position[0]);

        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].constant").c_str());
        glUniform1f(lp, lights.pointLights[i].constant);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].linear").c_str());
        glUniform1f(lp, lights.pointLights[i].linear);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].quadratic").c_str());
        glUniform1f(lp, lights.pointLights[i].quadratic);
    }

    //И направленные
    GLuint lns = glGetUniformLocation(shader_programme, "lights.numSLights");
    glUniform1i(lns, lights.numSLights);

    for (int i = 0; i < lights.numSLights; i++)
    {
        GLuint lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].ambient").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].ambient[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].diffuse").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].diffuse[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].specular").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].specular[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].position").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].position[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].direction").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].direction[0]);


        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].cutOff").c_str());
        glUniform1f(lsp, lights.spotLights[i].cutOff);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].outerCutOff").c_str());
        glUniform1f(lsp, lights.spotLights[i].outerCutOff);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].constant").c_str());
        glUniform1f(lsp, lights.spotLights[i].constant);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].linear").c_str());
        glUniform1f(lsp, lights.spotLights[i].linear);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].quadratic").c_str());
        glUniform1f(lsp, lights.spotLights[i].quadratic);
    }

    //А ещё нужны координаты камеры
    GLuint campos = glGetUniformLocation(shader_programme, "campos");
    glUniform3fv(campos, 1, &CamPosition[0]);


    //Теперь материал если он есть
    if (modelMode == 1)
    {
        GLuint ma = glGetUniformLocation(shader_programme, "material.ambient");
        glUniform3fv(ma, 1, &material.ka[0]);
        GLuint md = glGetUniformLocation(shader_programme, "material.diffuse");
        glUniform3fv(md, 1, &material.kd[0]);
        GLuint ms = glGetUniformLocation(shader_programme, "material.specular");
        glUniform3fv(ms, 1, &material.ks[0]);
        GLuint msh = glGetUniformLocation(shader_programme, "material.shininess");
        glUniform1f(msh, material.shininess);
    }
    
    if (modelMode == 2)
    {


        //Загрузка ссылки на текстуру в шейдер
        GLuint mtx = glGetUniformLocation(shader_programme, "material.texture");
        glUniform1i(mtx, 0);
        //Флаг есть ли карта отражений
        GLuint mm = glGetUniformLocation(shader_programme, "material.hasMap");
        if (texturemap)
            glUniform1i(mm, 1);
        else
            glUniform1i(mm, 0);

        GLuint ms = glGetUniformLocation(shader_programme, "material.specularmap");
        glUniform1i(ms, 1);

        //Активация текстур
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        if (texturemap != -1)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texturemap);
        }

        //   И от материала всё равно нужен уровень зеркальности
        GLuint msh = glGetUniformLocation(shader_programme, "material.shininess");
        glUniform1f(msh, material.shininess);
    }

    //А дальше всё так же как и в простом случае
    glBindVertexArray(vao);
    if (indices_count > 0)
        glDrawElements(mode, indices_count, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(mode, 0, verteces_count);
}

void Model::load_coords(glm::vec3* verteces, size_t count)
{
    //Сохранение размера массива
    verteces_count = count;

    //Создание VBO
    GLuint coords_vbo = 0;
    glGenBuffers(1, &coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), verteces, GL_STATIC_DRAW);

    //Так как VAO уже создан, то можно сразу связать с ним
    glBindVertexArray(vao);

    //Собственно связь
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //Подключение атрибута
    glEnableVertexAttribArray(0);

}

void Model::load_colors(glm::vec3* colors, size_t count)
{
    //Аналогично координатам
    //Размер сохранять не требуется - важно только число вершин
    GLuint colors_vbo = 0;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), colors, GL_STATIC_DRAW);

    glBindVertexArray(vao);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
}

void Model::load_indices(GLuint* indices, size_t count)
{
    //Сохранение числа индексов
    indices_count = count;

    //Дальше всё почти как всегда с поправкой на тип буфера
    glBindVertexArray(vao);

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer); // Генерация одного объекта буфера вершин
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer); // Привязка элементного буфера
    //Загрузка индексов в используемый элементный буфер
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices, GL_STATIC_DRAW);

}

void Model::load_normals(glm::vec3* normals, size_t count)
{
    GLuint normals_vbo = 0;
    glGenBuffers(1, &normals_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), normals, GL_STATIC_DRAW);

    glBindVertexArray(vao);

    //Так как при использовании нормалей цвет заменяется на материал или текстуры, то они назначаются в тот же массив
    //Стоит добавить сюда проверку и отключать возможность включения цветов
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

}

void Model::load_texcoord(glm::vec2* tex, size_t count)
{
    GLuint tex_vbo = 0;
    glGenBuffers(1, &tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec2), tex, GL_STATIC_DRAW);

    glBindVertexArray(vao);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

}

void Model::load_texture(string path, string pathtomap)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    //Так как карты отражений может не быть - применён вот такой метод
    //Если пути нет, она загружена не будет и её id останется 0
    //Так как она грузится второй, то id равный 0 означает что не загружена
    //В текстурном шейдере жёстко сказано что если карты отражений нет - считать что объект не отражает ничего
    //По хорошему можно для надёжности через флаг сделать или генерацию карты, но и так нормально
    if (pathtomap != "")
    {
        data = stbi_load(pathtomap.c_str(), &width, &height, &nrChannels, 0);
        glGenTextures(1, &texturemap);
        glBindTexture(GL_TEXTURE_2D, texturemap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
}

void Model::load_shaders(const char* vect, const char* frag)
{
    // Переменные под результат компиляции программы
    GLint result = GL_FALSE;
    int infoLogLength;

    //Создание шейдерной программы
    shader_programme = glCreateProgram();

    //Загрузка текстов шейдеров из файлов
    string vstext = LoadShader(vect);
    const char* vertex_shader = vstext.c_str();
    string fstext = LoadShader(frag);
    const char* fragment_shader = fstext.c_str();

    //Создание вершинного шейдера
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    //Проверка результата компиляции
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);
    //Вывод сообщения об ошибке если что-то пошло не так
    if (infoLogLength > 0)
    {
        char* errorMessage = new char[infoLogLength + 1];
        glGetShaderInfoLog(vs, infoLogLength, NULL, errorMessage);
        std::cout << errorMessage;
        delete errorMessage;
    }

    //Аналогично с фрагментным шейдером
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        char* errorMessage = new char[infoLogLength + 1];
        glGetShaderInfoLog(fs, infoLogLength, NULL, errorMessage);
        std::cout << errorMessage;
        delete errorMessage;
    }

    //Сборка программы
    glAttachShader(shader_programme, vs);
    glAttachShader(shader_programme, fs);

    
    glBindAttribLocation(shader_programme, 0, "vertex_position");
    //Либо нормаль, либо цвет, не лучший метод, но самый простой
    if (!modelMode)
        glBindAttribLocation(shader_programme, 1, "vertex_color");
    else
        glBindAttribLocation(shader_programme, 1, "vertex_normal");
    //Если есть текстура, её тоже надо прикрепить
    if(modelMode==2)
        glBindAttribLocation(shader_programme, 2, "vertex_texture");


    //Компоновка шейдерной программы
    glLinkProgram(shader_programme);

}

void Model::setMaterial(glm::vec3 a, glm::vec3 d, glm::vec3 s, GLfloat shine)
{
    material.ka = a;
    material.kd = d;
    material.ks = s;
    material.shininess = shine;
}