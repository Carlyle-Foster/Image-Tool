#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
float objectWeight = 0.35;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code
    float a = texelColor.a;
    vec4 objectColor = vec4(0.0);
    float r = (a + 273.2) * 37.2;
    objectColor.r = r - floor(r);
    float g = (a + 12.2) * 72.45 ;
    objectColor.g = g - floor(g);
    float b = (a + 102.8) * 231.45;
    objectColor.b = b - floor(b);
    texelColor = texelColor*(1.0 - objectWeight) + objectColor*objectWeight;

    finalColor = texelColor*colDiffuse;
}