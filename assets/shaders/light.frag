#version 450 core

layout(location = 0) out vec4 color;

// Bitmap for textures present in order of below
// e.g. 0001 for ambient
uniform int u_texturesPresent;
uniform sampler2D u_ambientTex;
uniform sampler2D u_diffuseTex;
uniform sampler2D u_specTex;
uniform sampler2D u_shinyTex;
uniform sampler2D u_bumpTex;

// Material properties
uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specColor;
uniform float u_shininess;
uniform float u_refractionIndex;
uniform float u_alpha;

struct LightSource {
    int type;
    vec3 pos;
    vec3 dir;
    vec3 color;
    vec3 intensities;
};

// Scene properties
readonly layout(std430, binding=2) buffer Scene {
    vec3 u_cameraPos;
    LightSource lights[];
};

// Interpolated coords from vertex shader
in vec3 v_pos;
in vec2 v_textureUV;
in vec3 v_normal;

void main() {
    // Load texture data
    vec3 ambientTex = vec3(1.0f);
    vec3 diffuseTex = vec3(1.0f);
    vec3 specTex = vec3(1.0f);
    float shinyTex = 1.0f;
    vec3 bumpTex = vec3(1.0f);
    if ((u_texturesPresent & 1) >= 1)
        ambientTex = vec3(texture(u_ambientTex, v_textureUV));
    if ((u_texturesPresent & 2) >= 1)
        diffuseTex = vec3(texture(u_diffuseTex, v_textureUV));
    if ((u_texturesPresent & 4) >= 1)
        specTex    = vec3(texture(u_specTex, v_textureUV));
    if ((u_texturesPresent & 8) >= 1)
        shinyTex   = float(texture(u_shinyTex, v_textureUV));
    if ((u_texturesPresent & 16) >= 1)
        bumpTex    = vec3(texture(u_bumpTex, v_textureUV));

    vec3 ambientColor = u_ambientColor * ambientTex;
    vec3 diffuseColor = u_diffuseColor * diffuseTex;
    vec3 specColor = u_specColor * specTex;
    float shininess = u_shininess * shinyTex;

    vec3 normal = normalize(v_normal);

    color = vec4(vec3(gl_FragCoord), u_alpha);
}

vec3 calculateLight(LightSource light, vec3 normal,
vec3 ambientColor, vec3 diffuseColor, vec3 specColor, float shininess) {
    vec3 lightDir = normalize(light.pos - v_pos);

    vec3 ambient = light.color * light.intensities[0];
    vec3 diffuse = max(dot(normal, light.dir), 0) * light.color * light.intensities[1];

    vec3 viewDir = normalize(u_cameraPos-v_pos);
    /* For phong shading
    vec3 reflectionVec = reflect(-lightVec, normal);
    */
    vec3 halfDir = normalize(light.dir + viewDir);

    float specAngle = max(dot(halfDir, normal), 0);
    vec3 specular = pow(specAngle, shininess) * light.color * light.intensities[2];

    vec3 shading =  (ambient*ambientColor) +
    (diffuse*diffuseColor) +
    (specular*specColor);
    return shading;
}