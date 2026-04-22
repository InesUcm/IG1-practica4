#version 330 core

const vec4 ambient  = vec4(0.2,  0.2,  0.2,  1.0);
const vec4 diffuse  = vec4(0.75, 0.75, 0.75, 1.0);
const vec4 specular = vec4(1.0,  1.0,  1.0,  1.0);
const int  specularExponent = 13;

uniform vec4 lightDir;
uniform vec4 color;

in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

void main()
{
	vec3 normal = normalize(Normal);
	float diff_factor = max(dot(normal, -lightDir.xyz), 0.0);
	vec3 viewDir    = normalize(-FragPos);
	vec3 reflectDir = reflect(lightDir.xyz, normal);
	float spec_factor = pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
	FragColor = color * (ambient + diff_factor * diffuse + spec_factor * specular);
}