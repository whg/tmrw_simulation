#version 330

in vec4 position;
in vec4 colour;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

uniform mat4 transformMatrix;

out Vertex {
    vec4 colour;
} vertex;


void main() {
	
	gl_Position = modelViewMatrix * transformMatrix * position;
    vertex.colour = colour;
}