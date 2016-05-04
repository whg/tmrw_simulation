#version 330

in vec4 position;
in vec4 colour;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;


out Vertex {
    vec4 colour;
} vertex;

void main() {
	
	gl_Position = modelViewMatrix * position;
    vertex.colour = colour;
//    gl_Position = modelViewProjectionMatrix * position;
}