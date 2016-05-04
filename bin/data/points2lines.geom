#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in Vertex {
    vec4 colour;
} vertex[];


uniform mat4 projectionMatrix;

out vec2 texCoordVarying;
out vec4 colourVarying;

void main() {
	
    vec4 center = gl_in[0].gl_Position;
    vec4 other = gl_in[1].gl_Position;

//  the line given is treaded like this:
//     -------
//    |       |
//    |   o<->o
//    |       |
//     -------
    
    float halfSideLength = distance(center, other);
    vec4 offset = vec4(0, halfSideLength, 0, 1);
    
    
    
	gl_Position = projectionMatrix * (center + vec4(-halfSideLength, -halfSideLength, 0, 1));
    texCoordVarying = vec2(0, 0);
    colourVarying = vertex[0].colour;
	EmitVertex();
    
    gl_Position = projectionMatrix * (center + vec4(halfSideLength, -halfSideLength, 0, 1));;
    texCoordVarying = vec2(1, 0);
    colourVarying = vertex[1].colour;
    EmitVertex();
    
    gl_Position = projectionMatrix * (center + vec4(-halfSideLength, halfSideLength, 0, 1));
    texCoordVarying = vec2(0, 1);
    colourVarying = vertex[0].colour;
    EmitVertex();
    
    gl_Position = projectionMatrix * (center + vec4(halfSideLength, halfSideLength, 0, 1));
    texCoordVarying = vec2(1, 1);
    colourVarying = vertex[1].colour;
    EmitVertex();
    
	EndPrimitive();
	
}