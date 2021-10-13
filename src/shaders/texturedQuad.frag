#version 450

uniform sampler2D frameTexture;
in vec2 vertexShaderPosition;

void main()
{
    vec2 texCoords = 0.5 * (vertexShaderPosition + 1.0);
    gl_FragColor = vec4(texture2D(frameTexture, texCoords).rgb * 0.98, 1.0);
}