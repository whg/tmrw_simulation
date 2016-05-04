#version 330

uniform sampler2DRect tex;
uniform vec2 texSize;

in vec2 texCoordVarying;
in vec4 colourVarying;

out vec4 fragColor;

void main() {

    fragColor = texture(tex, texCoordVarying * texSize) * colourVarying;
//    fragColor = colourVarying;
}