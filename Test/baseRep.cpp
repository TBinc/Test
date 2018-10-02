#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstdio>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_utils.h"

//GLuint vbo_object;
//	GLuint ibo_object;

GLuint program;
GLint attribute_coord;

GLint uniform_mvp;
GLint uniform_xmin;
GLint uniform_xmax;
GLint uniform_ymin;
GLint uniform_ymax;
GLint uniform_zmin;
GLint uniform_zmax;

GLfloat t = 0;
GLfloat d = 0;

int mov = 0;
int screen_width = 800, screen_height = 800;

//#define MAXV 100000
//#define MAXT 100000

//GLfloat vertices[MAXV][3];
//GLushort triangles[MAXT][3];

typedef struct Surface{
	GLfloat* vertices;
	GLushort* triangles;
	int numVertices;
	int numTriangles;

	GLuint vbo_object;
	GLuint ibo_object;
}TSurface;

TSurface surface1;
TSurface surface2;
TSurface surface3;

float xmin=1e09, xmax=-1e09, ymin=1e09, ymax=-1e09, zmin=1e09, zmax=-1e09;

int numVertices = 0;
int numTriangles = 0;

void read_file(const char* filename, TSurface* T){
    FILE* fp = fopen(filename, "rt");
    char buffer[100];

    //Leer formato
    fscanf(fp, "%s", buffer);
    int aux;

    //Leer #vertices, #triangulos, #aristas
    fscanf(fp, "%d %d %d", &T->numVertices, &T->numTriangles, &aux);

    T->vertices = new GLfloat[T->numVertices * 3];
    T->triangles = new GLushort[T->numTriangles * 3];

    for(int i = 0; i < T->numVertices; i++){
    	GLfloat x, y, z;
        fscanf(fp, "%f %f %f", &x, &y, &z);
        if(x < xmin)	xmin = x;
        if(x > xmax)	xmax = x;
        if(y < ymin)	ymin = y;
        if(y > ymax)	ymax = y;
        if(z < zmin)	zmin = z;
        if(z > zmax)	zmax = z;

        T->vertices[3 * i] = x;
        T->vertices[3 * i + 1] = y;
        T->vertices[3 * i + 2] = z;
    }

    for(int i = 0; i < T->numTriangles; i++){
        fscanf(fp, "%d %d %d %d", &aux, &T->triangles[3 * i], &T->triangles[3 * i+1], &T->triangles[3 * i+2]);
    }

    fclose(fp);
}


bool init_resources(){

    read_file("avion.off", &surface1);
    read_file("helice.off", &surface2);
	read_file("rifle.off", &surface3);

    glGenBuffers(1, &surface1.vbo_object);
    glBindBuffer(GL_ARRAY_BUFFER, surface1.vbo_object);
    glBufferData(GL_ARRAY_BUFFER, surface1.numVertices * 3 * sizeof(GLfloat), surface1.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &surface1.ibo_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface1.ibo_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 surface1.numTriangles * 3 * sizeof(GLushort), surface1.triangles, GL_STATIC_DRAW);

	glGenBuffers(1, &surface2.vbo_object);
    glBindBuffer(GL_ARRAY_BUFFER, surface2.vbo_object);
    glBufferData(GL_ARRAY_BUFFER, surface2.numVertices * 3 * sizeof(GLfloat), surface2.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &surface2.ibo_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface2.ibo_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 surface2.numTriangles * 3 * sizeof(GLushort), surface2.triangles, GL_STATIC_DRAW);

	glGenBuffers(1, &surface3.vbo_object);
	glBindBuffer(GL_ARRAY_BUFFER, surface3.vbo_object);
	glBufferData(GL_ARRAY_BUFFER, surface3.numVertices * 3 * sizeof(GLfloat), surface3.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &surface3.ibo_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface3.ibo_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		surface3.numTriangles * 3 * sizeof(GLushort), surface3.triangles, GL_STATIC_DRAW);
    
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    if((vs = create_shader("basic.v.glsl", GL_VERTEX_SHADER))==0) return false;
    if((fs = create_shader("basic.f.glsl", GL_FRAGMENT_SHADER))==0) return false;

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if(!link_ok){
        std::cout << "Problemas con el Shader" << std::endl;
        return false;
    }

    attribute_coord = glGetAttribLocation(program, "coord");
    if(attribute_coord == -1){
        std::cout << "No se puede asociar el atributo coord" << std::endl;
        return false;
    }

    uniform_mvp = glGetUniformLocation(program, "mvp");
    if(uniform_mvp == -1){
        std::cout << "No se puede asociar el uniform mvp" << std::endl;
        return false;
    }


	uniform_xmax = glGetUniformLocation(program, "xmax");
    if(uniform_xmax == -1){
        std::cout << "No se puede asociar el uniform xmax" << std::endl;
        return false;
    }

    uniform_xmin = glGetUniformLocation(program, "xmin");
    if(uniform_xmin == -1){
        std::cout << "No se puede asociar el uniform xmin" << std::endl;
        return false;
    }

    uniform_ymax = glGetUniformLocation(program, "ymax");
    if(uniform_ymax == -1){
        std::cout << "No se puede asociar el uniform ymax" << std::endl;
        return false;
    }

    uniform_ymin = glGetUniformLocation(program, "ymin");
    if(uniform_ymin == -1){
        std::cout << "No se puede asociar el uniform ymin" << std::endl;
        return false;
    }

    uniform_zmax = glGetUniformLocation(program, "zmax");
    if(uniform_zmax == -1){
        std::cout << "No se puede asociar el uniform zmax" << std::endl;
        return false;
    }

    uniform_zmin = glGetUniformLocation(program, "zmin");
    if(uniform_zmin == -1){
        std::cout << "No se puede asociar el uniform zmin" << std::endl;
        return false;
    }
    return true;
}

void animacion() {
	t = t + 0.005;
	switch (mov)
	{
	case 1:
		d = d + 0.001;
		break;
	case -1:
		d = d - 0.001;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

glm::vec3 camPosition(0.0f, 0.0f, 0.0f);
GLfloat pitch = 0.0f;
GLfloat yaw = -90.0f;
glm::vec3 camUp(0.0f, 1.0f, 0.0f);
glm::vec3 camRight;

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		pitch += 2.0f;
		pitch = (pitch > 89.0f) ? 89.0f : pitch;
		break;
	case 's':
		pitch -= 2.0f;
		pitch = (pitch < -89.0f) ? -89.0f : pitch;
		break;
	case 'a':
		yaw -= 2.0f;
		break;
	case 'd':
		yaw += 2.0f;
		break;
	case 'z':
		mov = 1;
		break;
	case 'x':
		mov = 0;
		break; 
	case 'c':
		mov = -1;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void onDisplay() {

	glm::vec3 front;
	front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
	front = glm::normalize(front);

	camRight = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	camUp = glm::normalize(glm::cross(camRight, front));


	//Creamos matrices de modelo, vista y proyeccion
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 view = glm::lookAt(camPosition + front * d, camPosition + front, camUp);
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width / screen_height, 0.1f, 2000.0f);
	glm::mat4 mvp = projection * view * model;

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	//Enviamos la matriz que debe ser usada para cada vertice
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform1f(uniform_xmax, xmax);
	glUniform1f(uniform_xmin, xmin);
	glUniform1f(uniform_ymax, ymax);
	glUniform1f(uniform_ymin, ymin);
	glUniform1f(uniform_zmax, zmax);
	glUniform1f(uniform_zmin, zmin);

	{
	glEnableVertexAttribArray(attribute_coord);

	glBindBuffer(GL_ARRAY_BUFFER, surface1.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface1.ibo_object);
	glDrawElements(GL_TRIANGLES, surface1.numTriangles * 3, GL_UNSIGNED_SHORT, 0);


	glDisableVertexAttribArray(attribute_coord);

	model = glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 0.0f, 2.0f)) *
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
	//glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 1.0f, 0.0f)) *
	//glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)) *
	//glm::rotate(glm::mat4(1.0), t, glm::vec3(2.0f, 1.0f, 3.0f));
	mvp = projection * view * model;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));


	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, surface2.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface2.ibo_object);
	glDrawElements(GL_TRIANGLES, surface2.numTriangles * 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord);


	model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -20.0f));
	mvp = projection * view * model;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));


	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, surface1.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface1.ibo_object);
	glDrawElements(GL_TRIANGLES, surface1.numTriangles * 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, -20.0f));
	mvp = projection * view * model;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

	
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, surface1.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface1.ibo_object);
	glDrawElements(GL_TRIANGLES, surface1.numTriangles * 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -20.0f)) *
		glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 0.0f, 2.0f));
	//glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 1.0f, 0.0f)) *
	//glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)) *
	//glm::rotate(glm::mat4(1.0), t, glm::vec3(2.0f, 1.0f, 3.0f));
	mvp = projection * view * model;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));


	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, surface2.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface2.ibo_object);
	glDrawElements(GL_TRIANGLES, surface2.numTriangles * 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, -20.0f)) *
		glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 0.0f, 2.0f));
	//glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 1.0f, 0.0f)) *
	//glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)) *
	//glm::rotate(glm::mat4(1.0), t, glm::vec3(2.0f, 1.0f, 3.0f));
	mvp = projection * view * model;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));


	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, surface2.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface2.ibo_object);
	glDrawElements(GL_TRIANGLES, surface2.numTriangles * 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord);
}
	//RIFLE
	model =
		glm::translate(glm::mat4(1.0f), camPosition + front * d) *
		glm::rotate(glm::mat4(1.0), -glm::radians(yaw+90.0f) , glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0), 3.141592f / 2, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0), 3.141592f, glm::vec3(0.0f, 1.0f, 0.0f))*
		glm::scale(glm::mat4(1.0), glm::vec3(0.1f, 0.1f, 0.1f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, -10.0f));
	/*model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0), 3.141592f / 2, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0), 3.141592f, glm::vec3(0.0f, 1.0f, 0.0f));
	/*glm::rotate(glm::mat4(1.0), t, glm::vec3(0.0f, 1.0f, 0.0f)) *
	glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)) *
	glm::rotate(glm::mat4(1.0), t, glm::vec3(2.0f, 1.0f, 3.0f));*/
	mvp = projection * view * model ;

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));


	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, surface3.vbo_object);

	glVertexAttribPointer(
		attribute_coord,
		3,
		GL_FLOAT,
		GL_FALSE,
		0, 0
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surface3.ibo_object);
	glDrawElements(GL_TRIANGLES, surface3.numTriangles * 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord);



    glutSwapBuffers();
}

void onReshape(int w, int h){
    screen_width = w;
    screen_height = h;

    glViewport(0,0,screen_width, screen_height);
}

void free_resources(){
    glDeleteProgram(program);
    glDeleteBuffers(1, &surface1.vbo_object);
    glDeleteBuffers(1, &surface1.ibo_object);
    glDeleteBuffers(1, &surface2.vbo_object);
    glDeleteBuffers(1, &surface2.ibo_object);
	glDeleteBuffers(1, &surface3.vbo_object);
	glDeleteBuffers(1, &surface3.ibo_object);

    delete[] surface1.vertices;
    delete[] surface1.triangles;
    delete[] surface2.vertices;
    delete[] surface2.triangles;
	delete[] surface3.vertices;
	delete[] surface3.triangles;

}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitContextVersion(2,0);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(screen_width, screen_height);
    glutCreateWindow("OpenGL");

    GLenum glew_status = glewInit();
    if(glew_status != GLEW_OK){
        std::cout << "Error inicializando GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!GLEW_VERSION_2_0){
        std::cout << "Tu tarjeta grafica no soporta OpenGL 2.0" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(init_resources()){
        glutDisplayFunc(onDisplay);
        glutReshapeFunc(onReshape);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glutIdleFunc(animacion);
		glutKeyboardFunc(keyboard);
		glutMainLoop();
    }

    free_resources();
    exit(EXIT_SUCCESS);
}
