#version 130

in vec4 aPosition;
in vec2 aTexCoord;
in vec4 aNormal;


out vec2 texCoord;
out vec4 vNormal;
out vec4 toCamera;

uniform mat4 projection, model, view;

void main() {
    
    gl_Position = projection * view * model * aPosition;
    texCoord = aTexCoord;
    vNormal = model * aNormal;

    toCamera = ((view) * vec4(0, 0, 0, 1));
}

#fragment_shader
#version 130

in vec2 texCoord;
in vec4 vNormal;
in vec4 toCamera;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec4 light_pos, light_color, blend_color;

float brightness = 1;

uniform int enable_texture, enable_blend;
uniform float shine;

void main() {
    vec4 unitLight = normalize(light_pos);
    vec4 unitNormal = normalize(vNormal);

    //if (nDot <= 0.5) nDot = 0.3;toCamera

    vec4 reflection = reflect(unitLight, unitNormal);
    float nDot = (max(dot(unitNormal, unitLight), 0.2) * brightness) * max(dot(reflection, toCamera), 0.2) * shine;

    //float specular = dot(reflection, toCamera) * shine;

    if(enable_texture == 1) {
        vec4 tex_color = texture(ourTexture, texCoord);
        if (enable_blend == 1 && blend_color == tex_color) discard;

        FragColor = tex_color * vec4(nDot, nDot, nDot, 1) * light_color;
        return;
    } else {
        FragColor = vec4(nDot, nDot, nDot, 1) * light_color;
    }

    //FragColor = vNormal;
}