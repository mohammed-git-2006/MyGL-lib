#version 130
in vec4 aPosition;
in vec2 aTexCoord;
in vec3 aNormal;

uniform mat4 view, model, projection;

out vec2 texCoord;
out vec3 normal;

void main() {
    gl_Position = projection * model * view * aPosition;

    normal = aNormal;
    texCoord = aTexCoord;
}

#fragment_shader
#version 130

out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;

uniform sampler2D ourTexture;
uniform vec4 light_pos, light_color, blend_color;
uniform float brightness;

void main() {
    vec4 tex_color = texture(ourTexture, texCoord);

    if (blend_color == tex_color) discard;

    vec4 uNormal = normalize(normal);
    vec4 uLightPos = normalize(light_pos);
    float nDot = max(dot(uNormal, uLightPos), 0.3) * brightness;
    FragColor = tex_color * vec4(nDot, nDot, nDot, 1) * light_color;
    //FragColor = vec4(1, 1, 1, 1) * vec4(nDot, nDot, nDot, 1) * light_color;
}