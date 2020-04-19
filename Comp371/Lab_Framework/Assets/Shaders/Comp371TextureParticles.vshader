#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 worldMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);  // default value for view matrix (identity)
uniform mat4 projectionMatrix = mat4(1.0);

uniform float density = .014f;
const float gradient = 1.5;

uniform vec3 aColor = vec3(1.0f, 1.0f, 1.0f);

out vec4 vertexColor;
out vec2 vertexUV;

out vec4 ioEyeSpacePosition;
out float visibility;

void main()
{

    mat4 mvMatrix = viewMatrix * worldMatrix;
    ioEyeSpacePosition = mvMatrix * vec4(aPos, 1.0f);

    float distance = length(ioEyeSpacePosition.xyz);
    visibility = exp(-pow((distance * density), gradient));
    visibility = clamp(visibility, 0.0f, 1.0f);


    mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;
    gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColor = vec4(aColor.r, aColor.g, aColor.b, 1.0f);
    vertexUV = aUV;
}