#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D myTexture;

void main() {
	vec3 lightColor = vec3(1.0);

	vec3 ambient = 0.1 * lightColor;

	vec3 norm = normalize(Normal);
	vec3 light = normalize(vec3(-2.0, 1.0, -0.5));
	float diff = max(dot(norm, light), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 color = ambient + diffuse;
	vec4 texColor = texture(myTexture, TexCoord);
	vec3 result = texColor.rgb * color;

	result = pow(result, vec3(1.0 / 2.2));

	FragColor = vec4(result, texColor.a);
}

