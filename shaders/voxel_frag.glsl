#version 330 core

in vec2 TexCoord;
out vec4 fragColorOut;

uniform sampler2D textureAtlas;

void main()
{
    vec4 texColor = texture(textureAtlas, TexCoord);
    fragColorOut = texture(textureAtlas, TexCoord);
}
