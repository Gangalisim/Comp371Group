#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
		
uniform mat4 worldMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
uniform mat4 projectionMatrix = mat4(1.0);
uniform vec3 aColor = vec3(1.0f, 1.0f, 1.0f);
		
out vec4 vertexColor;
void main()
{
   vertexColor = vec4(aColor.r, aColor.g, aColor.b, 1.0f);
   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}