#version 330 core
in vec4 vertexColor;
in vec3 Normal;
in vec3 FragPos;
in float visibility;

in vec4 fragment_position_light_space;
in vec4 ioEyeSpacePosition;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float angleZ = 0.0;
uniform vec3 viewPos;
uniform float shininess = 64.0;
uniform float specularStrength = 0.5;
uniform float ambientStrength = 0.1;

uniform int fogEnabled;
uniform sampler2D shadowMap;
		
out vec4 FragColor;

float ShadowCalculation(vec4 fragment_position_light_space)
{
	// perform perspective divide
    vec3 projCoords = fragment_position_light_space.xyz / fragment_position_light_space.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = 0.003;
    float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

    return shadow;
}

void main()
{
	

   //  ------------------Ambient------------------- //
   vec3 ambient = ambientStrength * lightColor; // ambientStrength * light color (white)
		   
   //  ------------------Diffuse------------------- //
   vec3 normalRotate = vec3((Normal.x * cos(angleZ)) + (Normal.z * sin(angleZ)), Normal.y, (-Normal.x * sin(angleZ)) + (Normal.z * cos(angleZ)));
   vec3 norm = normalize(normalRotate);
   vec3 lightDir = normalize(lightPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
		   
   //  ------------------Specular------------------- //
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm); 
		   		   
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
   vec3 specular = specularStrength * spec * lightColor;
   
   //  ------------------Combined------------------- //
   float shadow = ShadowCalculation(fragment_position_light_space);  
   

 
    
   vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(vertexColor);
  
   FragColor = vec4(result, 1.0);
   if (fogEnabled == 1)
   {
   FragColor = mix(vec4(0.345, 0.505, 0.552, 1.0f), FragColor, visibility);
   }
}