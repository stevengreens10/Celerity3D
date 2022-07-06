#version 460 core

layout(location = 0) out vec4 color;

// Bitmap for textures present in order of below
// e.g. 0001 for ambient
uniform int u_texturesPresent;
uniform sampler2D u_ambientTex;
uniform sampler2D u_diffuseTex;
uniform sampler2D u_specTex;
uniform sampler2D u_shinyTex;

uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specColor;
uniform float u_shininess;
uniform float u_refractionIndex;
uniform float u_alpha;

uniform vec4 u_color;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
// ambient, diffuse, specular
uniform vec3 u_lightIntensities;

in vec3 v_pos;
in vec2 v_textureUV;
in vec3 v_normal;

void main() {
    // Load texture data
    vec3 ambientTex = vec3(1.0f);
    vec3 diffuseTex = vec3(1.0f);
    vec3 specTex = vec3(1.0f);
    float shinyTex = 1.0f;
    if ((u_texturesPresent & 1) == 1)
    ambientTex = vec3(texture(u_ambientTex, v_textureUV));
    if ((u_texturesPresent & 2) == 1)
    diffuseTex = vec3(texture(u_diffuseTex, v_textureUV));
    if ((u_texturesPresent & 4) == 1)
    specTex    = vec3(texture(u_specTex, v_textureUV));
    if ((u_texturesPresent & 8) == 1)
    shinyTex   = float(texture(u_shinyTex, v_textureUV));

    vec3 ambientColor = u_ambientColor * ambientTex;
    vec3 diffuseColor = u_diffuseColor * diffuseTex;
    vec3 specColor = u_specColor * specTex;
    float shininess = u_shininess * shinyTex;

    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_pos);

    vec3 ambient = u_lightColor * u_lightIntensities[0];
    vec3 diffuse = max(dot(normal, lightDir), 0) * u_lightColor * u_lightIntensities[1];

    vec3 viewDir = normalize(u_cameraPos-v_pos);
    /* For phong shading
    vec3 reflectionVec = reflect(-lightVec, normal);
    */
    vec3 halfDir = normalize(lightDir + viewDir);

    float specAngle = max(dot(halfDir, normal), 0);
    vec3 specular = pow(specAngle, shininess) * u_lightColor * u_lightIntensities[2];

    vec3 shading =  (ambient*ambientColor) +
    (diffuse*diffuseColor) +
    (specular*specColor);
    color = vec4(shading, u_alpha);
}