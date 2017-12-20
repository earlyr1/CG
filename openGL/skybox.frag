#version 330

out vec4 outColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
    outColor = texture(ourTexture, TexCoord);
    //outColor = vec4(0.5, 0.5, 0, 0);
}