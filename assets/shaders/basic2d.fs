#version 330 core

in vec2 TexCoord;
uniform sampler2D tex;
uniform bool useTextureAlpha;
uniform bool useTexture;
uniform vec4 color;
out vec4 FragColor;

void main() {
	float alpha = texture(tex, TexCoord).r;
	if (useTextureAlpha)
		FragColor = vec4(color.rgb, color.a * alpha);
	else if (useTexture)
		FragColor = texture(tex, TexCoord);
	else
		FragColor = color;
}
