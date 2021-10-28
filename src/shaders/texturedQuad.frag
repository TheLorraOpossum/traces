R"(
#version 450

uniform sampler2D frameTexture;
in vec2 vertexShaderPosition;
uniform float standardDeviation;
uniform float fadeFactor;
uniform float xCorrection;
out vec4 color;

float PI = 3.14159265359;
float E = 2.71828182846;

float gaussian(in vec2 p, in float sigmaSquare)
{
    float exponent = - (p.x * p.x + p.y * p.y) / (2.0 * sigmaSquare);
    return (1.0 / 2.0 * PI * sigmaSquare) * exp(exponent);
}

void main()
{
    //failsafe so we can use turn off the blur by setting the deviation to 0
    vec2 texCoords = 0.5 * (vertexShaderPosition + 1.0);
    if(standardDeviation <= 0)
    {
        color = texture2D(frameTexture, texCoords);
    }
    else
    {
        float support = 1.0 * standardDeviation;
        float magnitudeStep = 0.5 * standardDeviation;

        float sigmaSquare = standardDeviation * standardDeviation;

        float factor = gaussian(vec2(0), sigmaSquare);
        vec3     rgb = texture2D(frameTexture, texCoords).rgb * factor;
        float    sum = factor;

        for (float magnitude = magnitudeStep; magnitude <= support / 2.0; magnitude += magnitudeStep)
        {
            for (float theta = 0; theta <= 2 * PI; theta += PI/2.0)
            {
                vec2 v = magnitude * vec2(xCorrection * cos(theta), sin(theta));
                factor = gaussian(v, sigmaSquare);
                rgb += texture2D(frameTexture, texCoords + v).rgb * factor;
                sum += factor;
            }
        }

        float makesItBrighter = 1.2; // CHEAT, but otherwise the traces are too faint
        color = vec4(rgb/sum * makesItBrighter, 1.0);
    }
    color = vec4(color.rgb * fadeFactor, 1.0);
}
)"
