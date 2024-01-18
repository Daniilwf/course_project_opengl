#include "Model.h"
#include "func.h"
#define PI 3.14159265358979323846f  

const glm::mat4 WorldMatrix = glm::mat4(1.0f);


int WinWidth;
int WinHeight;


// Позиция персонажа
glm::vec3 characterPosition(0, 1, 0);

// Позиция камеры относительно персонажа (отступ по оси Y и Z)
glm::vec3 cameraOffset(0.0f, 4.0f, -4.0f);

bool RotationMode;

glm::vec3 CamPosition;
glm::vec3 CamTarget = glm::vec3(0.0, 2.0, 0.0);
bool CamMoved = false;
GLfloat CamSpeed = 1;
GLfloat CharacterSpeed = 1;
GLfloat rotationAngle = -90;
GLfloat distanceToCamera = 9;
GLfloat moveSpeed = 0.5;

SpotLight left_headlight;
SpotLight right_headlight;

std::vector<Projectile> projectiles;

struct grass_coordinate {
    float x;
    float y;
    float z;

    grass_coordinate() {
        x = 0;
        y = 0;
        z = 0;
    }
};

struct tree_coordinate {
    float x;
    float y;
    float z;

    tree_coordinate() {
        x = 0;
        y = 0;
        z = 0;
    }
};

int main() {
    srand(time(nullptr));

    GLFWwindow* window = InitAll(1024, 768, false);
    if (window == nullptr)
    {
        EndAll();
        return -1;
    }

    //Время запуска программы, нужно для анимации.
    double start_time = glfwGetTime();

    //Три объекта - капибара, тор и сфера. Тор и сфера разноцветные, капибара - коричневая.

    //Массивы для хранения атрибутов вершин
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_textures;
    std::vector< glm::vec3 > temp_normals;
    std::vector< glm::vec3 > temp_colors;
    std::vector< GLuint > temp_indices;
    size_t size;
    float r, g, b;

    //Загрузка координат из файла
    loadOBJ("Tank.obj", temp_vertices, temp_textures, temp_normals);
    size = temp_vertices.size();
    //Создание модели
    Model tank(window,2);
    //Загрузка данных модели
    tank.load_coords(temp_vertices.data(), size);
    tank.load_normals(temp_normals.data(), temp_normals.size());
    tank.load_texcoord(temp_textures.data(), temp_textures.size());
    tank.load_shaders("texvs.glsl", "texfs.glsl");
    tank.load_texture("tank_texture.jpg", "");

    //Загрузка координат из файла
    loadOBJ("road/road.obj", temp_vertices, temp_textures, temp_normals);
    size = temp_vertices.size();
    //Создание модели
    Model road(window, 2);
    //Model road(window, 1);
    //Загрузка данных модели
    road.load_coords(temp_vertices.data(), size);
    road.load_normals(temp_normals.data(), temp_normals.size());
    road.load_texcoord(temp_textures.data(), temp_textures.size());
    road.load_shaders("texvs.glsl", "texfs.glsl");
    road.load_texture("road/road.jpg", "");

    const int grasscount = 50;
    std::vector<Model> grass(grasscount, Model(window, 1));
    std::vector<grass_coordinate> grassCoordinate(grasscount);
    for (int i = 0; i < grasscount; ++i) {
        float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 0.1f;
        glm::vec3 coord[] = { {0.0f, 0.0f, 0.0f}, {0.0f, y, 0.0f} };
        glm::vec3 colors[] = { {0.0f, 1.0f, 0.0f} };
        grass[i].load_coords(coord, 2);
        //rain[i].load_colors(colors, 2);
        grass[i].load_normals(coord, 2);
        grass[i].setMaterial(glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), 128 * 0.4);
        grass[i].load_shaders("matvs.glsl", "matfs.glsl");

        float x = rand() % 50 + 3;
        float z = rand() % 50 + 3;
        grassCoordinate[i].x = x;
        grassCoordinate[i].y = y;
        grassCoordinate[i].z = z;
    }

    genSphere(temp_indices, temp_vertices, temp_textures, temp_normals, 5, 50, 50);
    size = temp_vertices.size();
    Model sun(window, 1);
    sun.load_coords(temp_vertices.data(), size);
    sun.load_normals(temp_normals.data(), temp_normals.size());
    sun.load_texcoord(temp_textures.data(), temp_textures.size());
    sun.load_indices(temp_indices.data(), temp_indices.size());
    sun.setMaterial(glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 128 * 0.3);
    sun.load_shaders("matvs.glsl", "matfs.glsl");

    genSphere(temp_indices, temp_vertices, temp_textures, temp_normals, 1, 50, 50);
    size = temp_vertices.size();
    Model bullet(window, 1);
    bullet.load_coords(temp_vertices.data(), size);
    bullet.load_normals(temp_normals.data(), temp_normals.size());
    bullet.load_texcoord(temp_textures.data(), temp_textures.size());
    bullet.load_indices(temp_indices.data(), temp_indices.size());
    bullet.setMaterial(glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 128 * 0.3);
    bullet.load_shaders("matvs.glsl", "matfs.glsl");


    //Загрузка координат из файла
    loadOBJ("Tree_low.obj", temp_vertices, temp_textures, temp_normals);
    size = temp_vertices.size();
    //Создание модели
    const int treeCount = 25;
    std::vector<tree_coordinate> treeCoordinate(treeCount);
    std::vector<Model> trees(treeCount, Model(window, 1));
    for (int i = 0; i < treeCount; ++i) {
        trees[i].load_coords(temp_vertices.data(), size);
        trees[i].load_normals(temp_normals.data(), temp_normals.size());
        trees[i].load_texcoord(temp_textures.data(), temp_textures.size());
        trees[i].load_shaders("matvs.glsl", "matfs.glsl");
        trees[i].setMaterial(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1, 0.35, 0.1), glm::vec3(0.45, 0.55, 0.45), 128 * 0.25);
        float x = rand() % 50 + 3;
        float y = rand() % 50 + 1;
        float z = rand() % 50 + 3;
        treeCoordinate[i].x = x;
        treeCoordinate[i].y = y;
        treeCoordinate[i].z = z;

    }

    // Генерируем верхнюю грань куба
    genCubeTopFace(temp_indices, temp_vertices, temp_textures, temp_normals, 200.0f); // размер грани 10x10

    // Создаем модель
    Model cubeTop(window, 1);
    cubeTop.load_coords(temp_vertices.data(), temp_vertices.size());
    cubeTop.load_normals(temp_normals.data(), temp_normals.size());
    cubeTop.load_texcoord(temp_textures.data(), temp_textures.size());
    cubeTop.load_indices(temp_indices.data(), temp_indices.size());
    cubeTop.load_shaders("matvs.glsl", "matfs.glsl");

    //Параметры освещения сцены
    LightsInfo Lights;

    Lights.numPLights = 0;
    Lights.numSLights = 2;

    glm::vec3 lightPos = { 1,1,0 };
    glm::vec3 la = { 0.2f, 0.2f, 0.2f };
    glm::vec3 ld = { 0.5f, 0.5f, 0.5f };
    glm::vec3 ls = { 1,1,1 };


    DirLight lSource;
    lSource.ambient = la;
    lSource.diffuse = ld;
    lSource.specular = ls;
    lSource.direction = glm::vec3(0, -1, 0);

    Lights.dirLight = lSource;
    for (int i = 0; i < 0; ++i) {
        PointLight P;
        P.ambient = glm::vec3(0.4f, 0.4f, 0.4f);
        P.diffuse = glm::vec3(1, 1, 1);
        P.specular = glm::vec3(1, 1, 1);
        P.position = glm::vec3(0, 3, 1.0 + i * 10);
        P.constant = 1;
        P.linear = 0.09;
        P.quadratic = 0.032;
        Lights.pointLights.push_back(P);
    }
 
    left_headlight.position = characterPosition; 
    left_headlight.direction = glm::vec3(cos(glm::radians(rotationAngle + 180)), 0, sin(glm::radians(rotationAngle + 180)));
    left_headlight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    left_headlight.diffuse = glm::vec3(10.8f, 10.8f, 10.8f);
    left_headlight.specular = glm::vec3(11.0f, 11.0f, 11.0f);
    left_headlight.cutOff = glm::cos(glm::radians(12.5f));
    left_headlight.outerCutOff = glm::cos(glm::radians(15.0f));
    left_headlight.constant = 1;
    left_headlight.linear = 0.009;
    left_headlight.quadratic = 0.0032;

    Lights.spotLights.push_back(left_headlight);

    
    right_headlight.position = characterPosition; 
    right_headlight.direction = glm::vec3(cos(glm::radians(rotationAngle + 180)), 0, sin(glm::radians(rotationAngle + 180)));
    right_headlight.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    right_headlight.diffuse = glm::vec3(10.8f, 10.8f, 10.8f);
    right_headlight.specular = glm::vec3(11.0f, 11.0f, 11.0f);
    right_headlight.cutOff = glm::cos(glm::radians(12.5f));
    right_headlight.outerCutOff = glm::cos(glm::radians(15.0f));
    right_headlight.constant = 1;
    right_headlight.linear = 0.009;
    right_headlight.quadratic = 0.0032;

    Lights.spotLights.push_back(right_headlight);


    //Теперь создание матриц
    glm::mat4 MMatr = WorldMatrix;
    glm::mat4 VMatr = WorldMatrix;
    glm::mat4 PMatr = WorldMatrix;

    //Матрицу проецирования можно задать сразу же, менять её не надо будет
    PMatr = glm::perspective(
        glm::radians(67.0f), // Угол обзора
        (float)WinWidth / WinHeight,       // Соотношение сторон
        0.01f,              // Ближняя плоскость отсечения
        10000.0f             // Дальняя плоскость отсечения
    );

    //Теперь начальные значения матрицы вида
    
    CamPosition = cameraOffset;
    auto temp = characterPosition;
    temp.y += 2;
    VMatr = glm::lookAt(CamPosition, temp, glm::vec3(0, 1, 0));

    float rotation = 0;
    //Цвет фона
    glClearColor(0.4f, 0.4f, 0.4f, 0.4f);

    //И так как речь о 3d важно не забыть про буфер глубины (попробуйте убрать его)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    
    //Теперь рисование
    while (!glfwWindowShouldClose(window)) {

        double elapsed = (glfwGetTime() - start_time) / 2;
        glViewport(0, 0, WinWidth, WinHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Если камеру двигали - надо пересчитать матрицу вида
        if (CamMoved)
        {
            VMatr = WorldMatrix;
            temp = characterPosition;
            temp.y += 2;
            VMatr = glm::lookAt(CamPosition, temp, glm::vec3(0, 1, 0));
            CamMoved = false;
        }

        for (int i = 0; i < 10; i++) {
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(0.0, -0.275, i * 8.5));
            road.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(0.0, -0.275, -i * 8.5));
            road.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
        }


        Lights.spotLights[0] = left_headlight;
        Lights.spotLights[1] = right_headlight;

        MMatr = WorldMatrix;
        MMatr = glm::translate(MMatr, glm::vec3(0.0, -100.0, 0.0));
        cubeTop.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);

        MMatr = WorldMatrix;
        MMatr = glm::translate(MMatr, glm::vec3(100.0, 60.0, 100.0));
        sun.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);

        //Всё аналогично
        MMatr = WorldMatrix; 
        MMatr = glm::translate(MMatr, characterPosition);
        MMatr = glm::scale(MMatr, glm::vec3(0.05f, 0.05f, 0.05f));
        MMatr = glm::rotate(MMatr, glm::radians(-rotationAngle - 90), glm::vec3(0.0, 1.0, 0.0));
        tank.render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
       
        for (int i = 0; i < treeCount; i++) {

            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(treeCoordinate[i].x, 0.0, treeCoordinate[i].z));
            MMatr = glm::scale(MMatr, glm::vec3(0.1f, 0.1f, 0.1f));
            trees[i].render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(-treeCoordinate[i].x, 0.0, treeCoordinate[i].z));
            MMatr = glm::scale(MMatr, glm::vec3(0.1f, 0.1f, 0.1f));
            trees[i].render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(treeCoordinate[i].x, 0.0, -treeCoordinate[i].z));
            MMatr = glm::scale(MMatr, glm::vec3(0.1f, 0.1f, 0.1f));
            trees[i].render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(-treeCoordinate[i].x, 0.0, -treeCoordinate[i].z));
            MMatr = glm::scale(MMatr, glm::vec3(0.1f, 0.1f, 0.1f));
            trees[i].render(MMatr, VMatr, PMatr, Lights, GL_TRIANGLES);
        }

        for (int i = 0; i < grasscount; i++) {
            glLineWidth(3.0f);

            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(grassCoordinate[i].x, 0, grassCoordinate[i].z));
            grass[i].render(MMatr, VMatr, PMatr, Lights, GL_LINES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(-grassCoordinate[i].x, 0, grassCoordinate[i].z));
            grass[i].render(MMatr, VMatr, PMatr, Lights, GL_LINES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(grassCoordinate[i].x, 0, -grassCoordinate[i].z));
            grass[i].render(MMatr, VMatr, PMatr, Lights, GL_LINES);
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, glm::vec3(-grassCoordinate[i].x, 0, -grassCoordinate[i].z));
            grass[i].render(MMatr, VMatr, PMatr, Lights, GL_LINES);
        }

        updateProjectiles(elapsed);

        for (const auto& projectile : projectiles) 
        {
            MMatr = WorldMatrix;
            MMatr = glm::translate(MMatr, projectile.position);
            bullet.render(MMatr, VMatr, PMatr, Lights, GL_LINES);
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    return 0;
}
