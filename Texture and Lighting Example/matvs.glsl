#version 400
in vec3 vertex_position;
in vec3 vertex_normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


out vec3 normal;
out vec3 f_pos;

void main() {

   //Преобразование координат нормалей с учётом матрицы модели
	normal = mat3(transpose(inverse(M))) * vertex_normal; ;
	//Преобразование координат фрагмента с учётом матрицы модели
	f_pos = vec3(M * vec4(vertex_position, 1.0));
	//Обычное вычисление координат фрагмента
	gl_Position = P * V * M * vec4(vertex_position, 1.0);
}
