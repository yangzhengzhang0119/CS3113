#version 330

uniform sampler2D texture0;
uniform vec2 lightPosition;
uniform float visionRadius;

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 finalColor;

void main()
{
    // dist from player
    float dist = distance(lightPosition, fragPosition);
    
    float brightness;
    if (dist < visionRadius) {
        // inside -- bright
        brightness = 1.0;
    } else {
        // outside radius -- dark
        float falloff = (dist - visionRadius) / (visionRadius * 0.5);
        brightness = max(0.05, 1.0 - falloff);
    }
    
    vec4 texColor = texture(texture0, fragTexCoord);
    
    if (texColor.r + texColor.g + texColor.b < 0.01 && texColor.a < 0.01) {
        finalColor = vec4(brightness, brightness, brightness, 1.0);
    } else {
        finalColor = vec4(texColor.rgb * brightness, texColor.a);
    }
}
