#version 130

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;


out vec2 texCoord;
out vec4 vNormal;

uniform mat4 projection, model, view;

void main() {
    
    gl_Position = projection * view * model * aPosition;
    
    texCoord = aTexCoord;
    vNormal = aNormal;
}

#fragment_shader
#version 130

in vec2 texCoord;
in vec4 vNormal;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec4 light_pos, light_color, blend_color;
uniform float brightness;

uniform int enable_texture, enable_blend;

void main() {
    vec4 uNormal = normalize(vNormal);
    vec4 uLightPos = normalize(light_pos);
    float nDot = max(dot(uNormal, uLightPos), 0.3) * brightness;

    if(enable_texture == 1) {
        vec4 tex_color = texture(ourTexture, texCoord);
        if (enable_blend == 1 && blend_color == tex_color) discard;

        FragColor = tex_color * vec4(nDot, nDot, nDot, 1) * light_color;
        return;
    } else {
        FragColor = vec4(1, 1, 1, 1) * vec4(nDot, nDot, nDot, 1) * light_color;
    }
}