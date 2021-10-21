#version 450

in vec2 positionMonometric;
uniform mat2 toNormalCoordinates;

void main()
{
    gl_Position = vec4(toNormalCoordinates * positionMonometric, 0.0, 1.0);
}
