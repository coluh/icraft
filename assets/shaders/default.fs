#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D myTexture;
uniform vec3 light;
uniform vec2 uv_offset;
uniform bool use_uv_offset;

void main() {
	vec3 lightColor = vec3(1.0);

	vec3 ambient = 0.1 * lightColor;

	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, -light), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 color = ambient + diffuse;
	vec2 uv = use_uv_offset ? (TexCoord + uv_offset) : TexCoord;
	vec4 texColor = texture(myTexture, uv);
	vec3 result = texColor.rgb * color;

	result = pow(result, vec3(1.0 / 2.2));

	FragColor = vec4(result, texColor.a);
}

