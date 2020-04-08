#version 330 core

in vec3 FragPos;

in vec4 fragment_position_light_space;

uniform vec3 viewPos = vec3(1.0f, 1.0f, 1.0f);

in vec3 vertexColor;
out vec4 FragColor;
void main()
{
	FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
}