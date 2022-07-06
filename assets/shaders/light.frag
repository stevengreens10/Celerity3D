#version 460 core

layout(location = 0) out vec4 color;

uniform vec4 u_color;
// diffuse, specular, shininess
uniform vec3 u_material;
uniform vec3 u_cameraPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
// ambient, diffuse, specular
uniform vec3 u_lightIntensities;

in vec3 v_pos;
in vec3 v_normal;

void main() {
	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(u_lightPos - v_pos);

	vec3 ambient = u_material[0] * u_lightColor * u_lightIntensities[0];
	vec3 diffuse = u_material[1] * max(dot(normal, lightDir), 0) * u_lightColor * u_lightIntensities[1];

	vec3 viewDir = normalize(u_cameraPos-v_pos);
	/* For phong shading
	vec3 reflectionVec = reflect(-lightVec, normal);
	*/
	vec3 halfDir = normalize(lightDir + viewDir);

	float specAngle = max(dot(halfDir, normal), 0);
	vec3 specular = pow(specAngle, u_material[2]) * u_lightColor * u_lightIntensities[2];

	vec3 shading =  (ambient + diffuse + specular);
	color = vec4(shading, 1.0f) * u_color;
}