#version 450

uniform sampler2D frameTexture;
in vec2 vertexShaderPosition;
out vec4 color;

void main()
{
    vec2 texCoords = 0.5 * (vertexShaderPosition + 1.0);
    vec3 col = texture2D(frameTexture, texCoords).rgb * 0.985;
    color = vec4(col, 1.0);
}
