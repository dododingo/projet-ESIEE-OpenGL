#version 330 core

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
 

out vec4 color;
 

void main (void)
{   
	color = vec4(1,0.5f,0.2,0);
}

