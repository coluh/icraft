#version 330 core

in vec2 TexCoord;
uniform sampler2D tex;
uniform bool useTexture;
uniform vec4 color;
out vec4 FragColor;

void main() {
	float alpha = texture(tex, TexCoord).r;
	if (useTexture)
		FragColor = vec4(color.rgb, color.a * alpha);
	else
		FragColor = color;
}
