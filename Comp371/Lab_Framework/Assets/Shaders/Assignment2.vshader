#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
		
uniform mat4 worldMatrix = mat4(1.0);
uniform mat4 worldOrientationMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
uniform mat4 projectionMatrix = mat4(1.0);
uniform mat4 colorMatrix = mat4(1.0);
uniform mat4 lightSpaceMatrix;
		
out vec4 vertexColor;
out vec3 FragPos; 
out vec3 Normal;
out vec4 fragment_position_light_space;

void main()
{
   vertexColor = colorMatrix * vec4(aColor.r, aColor.g, aColor.b, 1.0f);
   Normal = aNormal;
   FragPos = vec3(worldMatrix * vec4(aPos, 1.0));
   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldOrientationMatrix * worldMatrix;
   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   fragment_position_light_space = lightSpaceMatrix * vec4(FragPos, 1.0);
}