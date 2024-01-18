#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>


using namespace std;

/// <summary>
/// Структура с данными про материал объекта. Содержит конструктор по умолчанию с настройками "изумруд"
/// </summary>
struct Material
{
    glm::vec3 ka; // коэф. фонового отражения (цвет фонового освещения)
    glm::vec3 kd; // коэф. диффузного отражения (цвет объекта)
    glm::vec3 ks; // коэф. зеркального блика
    GLfloat shininess; // показатель глянцевости
    // Значения по умолчанию
    Material()
    {
        ka = glm::vec3(0.0215, 0.1745, 0.0215);
        kd = glm::vec3(0.07568, 0.61424, 0.07568);
        ks = glm::vec3(0.633, 0.727811, 0.633);
        shininess = 128 * 0.6;
    };
};

/// <summary>
/// Структура с описанием параметров фонового освещения
/// </summary>
struct DirLight
{
    /// <summary>
    /// Направление
    /// </summary>
    glm::vec3 direction;
    //Параметры света
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
/// <summary>
/// Структура с описанием точечного источника света
/// </summary>
struct PointLight
{
    /// <summary>
    /// Координаты
    /// </summary>
    glm::vec3 position;
    //Затухание
    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
/// <summary>
/// Структура с описанием направленного источника света
/// </summary>
struct SpotLight
{
    //Местоположение и направление
    glm::vec3 position;
    glm::vec3 direction;
    //Радиус луча
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

/// <summary>
/// Структура с описанием источников света на сцене
/// </summary>
struct LightsInfo {
    /// <summary>
    /// Параметры фонового освещения
    /// </summary>
    DirLight dirLight;
    /// <summary>
    /// Массив с точечными источниками света
    /// </summary>
    std::vector<PointLight> pointLights;
    /// <summary>
    /// Массив с направленными источниками света
    /// </summary>
    std::vector<SpotLight> spotLights;

    //Количество точечных и направленных источников света
    int numPLights;
    int numSLights;

};

struct Projectile {
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float distanceTravelled;
    float maxDistance;

    Projectile(const glm::vec3& pos, const glm::vec3& dir, float spd, float maxDist)
        : position(pos), direction(dir), speed(spd), distanceTravelled(0.0f), maxDistance(maxDist) {}

    bool update(float deltaTime) {
        position += direction * speed * deltaTime;
        distanceTravelled += speed * deltaTime;
        return distanceTravelled < maxDistance;
    }
};


/// <summary>
/// Основной класс модели объекта. Содержит методы для загрузки параметров и рендеринга.
/// </summary>
class Model
{
public:
    /// <summary>
    /// Простой конструктор - создаёт массив вершин, пока пустой. И подключает окно.
    /// </summary>
    /// <param name="w">Указатель на окно.</param>
    /// <param name="mode">Тип модели. 0 - цветная, 1 - с материалом, 2 - с текстурой.</param>
    Model(GLFWwindow* w, GLuint mode = 0) {
        glGenVertexArrays(1, &vao);
        window = w;
        modelMode = mode;
    };

    //Деструктор. Так как динамической памяти нет, то он ничего не делает.
    ~Model() {};


    //Методы для отображения модели. Простой без параметров и набор с параметрами в зависимости от работы.
    //В рамках примера будут реализованы только первые два

    /// <summary>
    /// Рендер почти без параметров - просто отображает массив вершин.
    /// </summary>
    /// <param name="mode">Единственный параметр - режим рисования.</param>
    void render(GLuint mode = GL_TRIANGLES);
    /// <summary>
    /// Рендер с учётом комбинированной матрицы преобразований.
    /// </summary>
    /// <param name="MVPMatrix">Матрица преобразований.</param>
    /// <param name="mode">Режим рисования.</param>
    void render(glm::mat4 MVPMatrix, GLuint mode);
    /// <summary>
    /// Рендер с независимыми матрицами модели, вида и проекции.
    /// </summary>
    /// <param name="modelMatrix">Матрица модели.</param>
    /// <param name="viewMatrix">Матрица вида.</param>
    /// <param name="projMatrix">Матрица проекции.</param>
    /// <param name="mode">Режим рисования.</param>
    void render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix, GLuint mode);

    /// <summary>
    /// Рендер с поддержкой освещения
    /// </summary>
    /// <param name="modelMatrix">Матрица модели.</param>
    /// <param name="viewMatrix">Матрица вида.</param>
    /// <param name="projMatrix">Матрица проекции.</param>
    /// <param name="lights">Параметры освещения сцены.</param>
    /// <param name="mode">Режим рисования.</param>
    void render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix, LightsInfo lights ,GLuint mode);

    //Набор методов для загрузки составляющих массива вершин
    //В дальнейшем аналогично стоит добавлять новые функции

    /// <summary>
    /// Метод для загрузки координат вершин.
    /// </summary>
    /// <param name="verteces">Массив с координатами.</param>
    /// <param name="count">Размер массива.</param>
    void load_coords(glm::vec3* verteces, size_t count);
    /// <summary>
    /// Метод для загрузки цветов вершин.
    /// </summary>
    /// <param name="colors">Массив цветов.</param>
    /// <param name="count">Размер массива.</param>
    void load_colors(glm::vec3* colors, size_t count);

    /// <summary>
    /// Метод для загрузки массива индексов.
    /// </summary>
    /// <param name="indices">Массив индексов.</param>
    /// <param name="count">Размер массива.</param>
    void load_indices(GLuint* indices, size_t count);

    /// <summary>
    /// Метод для загрузки нормалей
    /// </summary>
    /// <param name="normals">Массив с нормалями.</param>
    /// <param name="count">Размер массива.</param>
    void load_normals(glm::vec3* normals, size_t count); 

    /// <summary>
    /// Метод для загрузки текстурных координат.
    /// </summary>
    /// <param name="tex">Массив с текстурными координатами.</param>
    /// <param name="count">Размер массива.</param>
    void load_texcoord(glm::vec2* tex, size_t count); 
    
    /// <summary>
    /// Метод для загрузки текстур.
    /// </summary>
    /// <param name="path">Путь к основной текстуре.</param>
    /// <param name="pathtomap">Путь к карте отражений. (может быть пустым)</param>
    void load_texture(string path, string pathtomap);

    /// <summary>
    /// Метод для загрузки шейдеров. С целью примера загружает только вершинный и фрагментный шейдеры
    /// В результате выполнения будет скомпонована шейдерная программа.
    /// </summary>
    /// <param name="vect">Путь к вершинному шейдеру</param>
    /// <param name="frag">Путь к фрагментному шейдеру</param>
    void load_shaders(const char* vect, const char* frag);

    /// <summary>
    /// Метод для задания параметров материала
    /// </summary>
    /// <param name="a">Фоновый свет.</param>
    /// <param name="d">Диффузный свет.</param>
    /// <param name="s">Отражённый свет.</param>
    /// <param name="shine">Глянцевость.</param>
    void setMaterial(glm::vec3 a, glm::vec3 d, glm::vec3 s, GLfloat shine);


    void load_skybox_shaders(const char* vertexShaderPath, const char* fragmentShaderPath);
    unsigned int load_skybox_texture(const std::vector<std::string>& faces);
    unsigned int load_cubemap(const std::vector<std::string>& faces);
    void render_skybox(unsigned int cubemapTexture);
private:

    /// <summary>
    /// ID массива вершин
    /// </summary>
    GLuint vao = -1;    //Начальное значение - максимальное число, заведомо невозможное
    /// <summary>
    /// Количество вершин
    /// </summary>
    size_t verteces_count = 0;
    /// <summary>
    /// Количество индексов
    /// </summary>
    size_t indices_count = 0;

    /// <summary>
    /// ID шейдерной программы
    /// </summary>
    GLuint shader_programme = -1;//Начальное значение - максимальное число, заведомо невозможное

    /// <summary>
    /// Указатель на окно
    /// </summary>
    GLFWwindow* window;

    /// <summary>
    /// Материал из которого состоит объект
    /// </summary>
    Material material;

    /// <summary>
    /// Вспомогательная переменная, устанавливающая тип модели - с цветами или материалом/текстурой
    /// </summary>
    GLuint modelMode = 0;

    /// <summary>
    /// Текстура
    /// </summary>
    GLuint texture = -1;
    /// <summary>
    /// Карта отражений
    /// </summary>
    GLuint texturemap = -1;
};

    