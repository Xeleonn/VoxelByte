#version 330 core

out vec4 FragColor;
in vec3 frag_color;

void main()
{
    float gradient = gl_FragCoord.y / 1080.0;  // adjust to window height

    float shade = mix(0.6, 1.0, gradient);

    FragColor = vec4(frag_color * shade, 1.0f);
}