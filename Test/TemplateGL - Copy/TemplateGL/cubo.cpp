#include <iostream>
#include <cstdlib>

using namespace std;

#include <GL\glew.h> //manejo de tarjeta grafica
#include <GL\freeglut.h> //graficacion
#include "shader_utils.h"

//manejo de 2D a 3D
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//bvo = vertex buffer object

GLuint vbo_vertices;
GLuint vbo_color;

GLint program;

GLint attribute_coord;
GLint attribute_color;
GLint uniform_mvp;

int screen_width = 512;
int screen_height = 512;

bool init_resources() {
	GLfloat buffer_vert[] = {
		//Triangulo 1
		1.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,

		//Triangulo 2
		1.0, 0.0, 0.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 0.0,

		//Triangulo 3
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		
		//Triangulo 4
		0.0, 1.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0, 0.0,

		//Triangulo 5
		1.0, 0.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 1.0,

		//Triangulo 6
		0.0, 0.0, 1.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 1.0,

		//Triangulo 7
		0.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		0.0, 1.0, 1.0,

		//Triangulo 8
		0.0, 1.0, 0.0,
		0.0, 1.0, 1.0,
		1.0, 1.0, 0.0,

		//Triangulo 9
		1.0, 0.0, 0.0,
		1.0, 0.0, 1.0,
		1.0, 1.0, 0.0,

		//Triangulo 10
		0.0, 1.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 1.0, 1.0,

		//Triangulo 11
		1.0, 1.0, 0.0,
		0.0, 1.0, 1.0,
		1.0, 1.0, 1.0,

		//Triangulo 12
		1.0, 0.0, 1.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 1.0,
	};
	GLfloat buffer_color[] = {
		//Triangulo 1 - rojo
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		//Triangulo 2 - verde
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		//Triangulo 3 - azul
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,

		//Triangulo 4 - negro feik
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,

		//Triangulo 1 - verde
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		//Triangulo 2 - azul
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,

		//Triangulo 3 - rojo
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		//Triangulo 4 - negro
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,

		//Triangulo 4 - amarillo
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,

		//Triangulo 4 - amarillo
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,

		//Triangulo 4 - negro
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,

		//Triangulo 4 - amarillo
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0
	};

	glGenBuffers(1, &vbo_vertices);// == new var de C++
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); //llamas a que buffer se van a ejecutar las consiguientes operaciones sobre el buffer habilitado
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_vert), buffer_vert, GL_STATIC_DRAW);//mando data al buffer

	glGenBuffers(1, &vbo_color);// == new var de C++
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color); //si no llamo, se sobre-escrebiria en el buffer habilitado previamente
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_color), buffer_color, GL_STATIC_DRAW);//mando data al buffer

	GLint link_ok = GL_FALSE;
	GLuint vs, fs; //vertex shader, fragment shader - vs:geometria, fg:pixels

	if ((vs = create_shader("basic3.v.glsl", GL_VERTEX_SHADER)) == 0) return false; //pasa los shaders al GPU
	if ((fs = create_shader("basic3.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false; //pasa los shaders al GPU

	program = glCreateProgram(); //ambos shaders juntados
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cout << "Problemas con el shader" << endl;
		return false;
	}
	attribute_coord = glGetAttribLocation(program, "coord"); // la ubicacion de coord
	if (attribute_coord == -1) {
		cout << "No se encuentra atributo coord" << endl;
		return false;
	}

	attribute_color = glGetAttribLocation(program, "color");
	if (attribute_color == -1) {
		cout << "No se encuentra atributo color" << endl;
		return false;
	}

	uniform_mvp = glGetUniformLocation(program, "mvp");
	if(uniform_mvp == -1) {
		cout << "No se encuentra uniform mvp" << endl;
		return false;
	}

	return true;
}

void onDisplay() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3D TRANSFORMACION DE VISTA Y PROYECCION
	//transformacion de vista
	glm::mat4 view = glm::lookAt(glm::vec3(-3.0f, -3.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // posicion del punto en donde se encuentra la camara, a donde mira, a donde apunta el lado superior de la camara
	//proyeccion
	glm::mat4 proj = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 10.0f); //creacion de frustrum
	
	glm::mat4 mvp = proj * view;

	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp)); //ando la matrix uniform_mvp al shader ... value combierte de matriz de 4x4 a aray de 16 elementos

	glEnableVertexAttribArray(attribute_coord); //habilitando el atributo que quiero llenar con datos
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); 
	glVertexAttribPointer(attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0); //como tiene que poner los datos del buffer

	glEnableVertexAttribArray(attribute_color); 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glVertexAttribPointer(attribute_color, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);

	glDrawArrays(GL_TRIANGLES, 0, 36); //12 puntos a dibujar

	glDisableVertexAttribArray(attribute_coord);
	glDisableVertexAttribArray(attribute_color);

	glutSwapBuffers();
}

void free_resources() {
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_color);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(2, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height)
		;
	glutCreateWindow("Tetrahedro");

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cout << "Error de inicializacion de GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	if (!GLEW_VERSION_2_0) {
		cout << "La tarjeta grafica no soporta OpenGL" << endl;
		exit(EXIT_FAILURE);
	}
	if (init_resources()) {
		glutDisplayFunc(onDisplay);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glutMainLoop();
	}

	free_resources();
	return EXIT_SUCCESS;
}

