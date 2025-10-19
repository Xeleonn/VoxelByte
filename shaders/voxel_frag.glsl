#version 330 core

out vec4 FragColor;
in vec3 ourColor;

void main()
{
    float gradient = gl_FragCoord.y / 1080.0;  // adjust to window height

    float shade = mix(0.6, 1.0, gradient);

    FragColor = vec4(ourColor * shade, 1.0f);
}