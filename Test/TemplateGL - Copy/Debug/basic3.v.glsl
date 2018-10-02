#version 450

in vec3 coord;
in vec3 color;
uniform mat4 mvp;

out vec3 f_color; 

void main(){
	gl_Position = mvp * vec4(coord, 1);
	f_color = color;
}