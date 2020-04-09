#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 cameraPosition;
uniform Material material;

#define LIGHTS_N 2
uniform Light pointLights[LIGHTS_N];

uniform samplerCube depthMap0;
uniform samplerCube depthMap1;
uniform float farPlane;
uniform bool shadows;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculationSingleLight(vec3 fragPos, Light light, samplerCube depthMap)
{
    vec3 fragToLight = fragPos - light.position;

    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(cameraPosition - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}
float ShadowCalculation(vec3 fragPos)
{
    return min(ShadowCalculationSingleLight(fragPos, pointLights[0], depthMap0), ShadowCalculationSingleLight(fragPos, pointLights[1], depthMap1));
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPosition - FragPos);

    vec3 result = vec3(0, 0, 0);
    for(int i = 0; i < LIGHTS_N; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    float gamma = 2.2;
    vec3 color = pow(result, vec3(1.0/gamma));
    FragColor = vec4(color, 1.0);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 color = texture(material.diffuse, TexCoords).rgb;

    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    float gamma = 2.2;
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff;// * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec;// * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    //float shadow = ShadowCalculation(FragPos) * 0.0;
    float shadow = ShadowCalculation(FragPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    return lighting;
    //return (ambient + diffuse + specular);
}
