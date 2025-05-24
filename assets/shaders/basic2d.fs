#version 330 core

in vec2 TexCoord;
uniform sampler2D tex;
uniform bool useTextureAlpha;
uniform bool useTexture;
uniform vec2 uv_offset;
uniform bool use_uv_offset;
uniform vec4 color;
out vec4 FragColor;

void main() {
	vec2 uv = use_uv_offset ? (TexCoord/16 + uv_offset) : TexCoord;
	float alpha = texture(tex, uv).r;
	if (useTextureAlpha)
		FragColor = vec4(color.rgb, color.a * alpha);
	else if (useTexture)
		FragColor = texture(tex, uv);
	else
		FragColor = color;
}
