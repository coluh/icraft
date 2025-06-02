#version 330 core
in vec2 vUV;

uniform sampler2D textureAtlas;
uniform vec2 uv_offset;
// uniform vec4 tintColor;

out vec4 FragColor;

void main() {
	vec4 color = texture(textureAtlas, vUV + uv_offset);
	if (color.a < 0.01) discard;
	FragColor = color;
}
