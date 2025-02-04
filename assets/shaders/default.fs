#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D myTexture;

void main() {
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	vec3 ambient = 0.2 * lightColor;

	vec3 norm = normalize(Normal);
	vec3 light = normalize(vec3(-2.0, 1.0, -0.5));
	float diff = max(dot(norm, light), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 color = ambient + diffuse;
	FragColor = texture(myTexture, TexCoord) * vec4(color, 1.0);
}

