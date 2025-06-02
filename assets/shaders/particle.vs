#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 particlePos;
uniform float size;

out vec2 vUV;

void main() {
	vec3 right = vec3(view[0][0], view[1][0], view[2][0]) * size;
	vec3 up = vec3(view[0][1], view[1][1], view[2][1]) * size;
	vec3 worldPos = particlePos + aPos.x * right + aPos.y * up;
	gl_Position = proj * view * vec4(worldPos, 1.0);
	vUV = aUV;
}
