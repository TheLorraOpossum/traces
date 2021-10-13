#version 450

in vec2 position;
out vec2 vertexShaderPosition;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    vertexShaderPosition = gl_Position.xy;
}