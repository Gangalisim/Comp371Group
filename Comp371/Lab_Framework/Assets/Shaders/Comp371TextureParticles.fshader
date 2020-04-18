#version 330 core
  
in vec4 vertexColor;
in vec2 vertexUV;

in float visibility;
uniform sampler2D textureSampler;


uniform int fogEnabled;

out vec4 FragColor;


void main()
{
   vec3 result = vec3(vertexColor);
   
   
   FragColor = vec4(result, 1.0);
   if (fogEnabled == 1)
   {
   FragColor = mix(vec4(0.345f, 0.505f, 0.552f, 1.0f), FragColor, visibility);
   }
}