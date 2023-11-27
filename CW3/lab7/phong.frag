#version 450 core

layout (location = 0) out vec4 fColour;

const int  LIGHT_TYPE_DIRECTION = 0;
const int  LIGHT_TYPE_POINT = 1;
const int  LIGHT_TYPE_SPOT = 2;

struct Light {
    int type;
    vec3 pos;
    vec3 color;
	float constAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
    vec3 spotDirection;
	float	spotCutoff;
};

in vec2 uv;
in vec3 nor;
in vec3 FragPosWorldSpace;
in vec4 FragPosProjectedLightSpace;

uniform sampler2D diffuseMap;

uniform sampler2D shadowMap;
uniform vec3 lightDirection;

uniform vec3 camPos;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;

uniform vec3 ambient;
uniform int lightsCount;
uniform Light lights[8];

float shadowOnFragment(vec4 FragPosProjectedLightSpace,vec3 n,vec3 lightDir)
{
    vec3 ndc = FragPosProjectedLightSpace.xyz/ FragPosProjectedLightSpace.w;
    vec3 ss = (ndc+1)*0.5;
    
    float fragDepth = ss.z;


    float litDepth = texture (shadowMap, ss.xy).r;

    float bias =  max(0.000015 *(1.0 - dot(n,lightDir)),0.000008);

    float shadow = 0.f;
    shadow = fragDepth > (litDepth+bias) ? 1.0 : 0.0;

    if(fragDepth >1)
        shadow = 0.f;

    return shadow;
}

vec3 LightColor(vec3 lightColor,vec3 n,vec3 lightDir,vec3 K_diffuse)
{
    vec3 camDirection = normalize(camPos - FragPosWorldSpace);
    vec3 NrefLight = reflect(-lightDir, n);

    vec3 diffuse = lightColor * max(dot(n,lightDir),0.0) * K_diffuse;
    vec3 specular = lightColor * pow(max(dot(camDirection, NrefLight), 0), Ns) * Ks;

    return diffuse + specular;
}

vec3 CalculateDirectionalLight(Light light,vec3 n,vec3 K_diffuse)
{
    vec3 lightDir = light.pos;
     float shadow = shadowOnFragment(FragPosProjectedLightSpace,n,lightDir);

    return LightColor(light.color,n,lightDir,K_diffuse) * (1.0f-shadow);
}
vec3 CalculatePointLight(Light light,vec3 n,vec3 K_diffuse)
{
    vec3 Nto_light = light.pos - FragPosWorldSpace;
    float dist = length(Nto_light);
    Nto_light = normalize(Nto_light);

    vec3 color = LightColor(light.color,n,Nto_light,K_diffuse);
    float attenuation = 1/(light.constAttenuation + light.linearAttenuation*dist + light.quadraticAttenuation*dist*dist);
    return color * attenuation;
}
vec3 CalculateSpotLight(Light light,vec3 n,vec3 K_diffuse)
{
    vec3 Nfrom_light = normalize(FragPosWorldSpace - light.pos);

   	float phi = cos(radians(light.spotCutoff));
	vec3 NSpotDir = normalize(light.spotDirection);
	float theta = dot(Nfrom_light,NSpotDir);

	if (theta > phi) {
        return CalculatePointLight(light,n,K_diffuse);
    }
    else {
        return vec3(0,0,0);
    }

}


void main()
{
    vec3 n = normalize(nor);
    vec3 K_diffuse = Kd * texture(diffuseMap, uv).xyz;

    vec3 color = ambient * Ka * K_diffuse;

    for(int i = 0; i < lightsCount; i ++) {
        Light light = lights[i];
        if(light.type == LIGHT_TYPE_DIRECTION) {
            color += CalculateDirectionalLight(light,n,K_diffuse);
        }
        else if(light.type == LIGHT_TYPE_POINT) {
            color += CalculatePointLight(light,n,K_diffuse);
        }
        else if(light.type == LIGHT_TYPE_SPOT) {
           color += CalculateSpotLight(light,n,K_diffuse);
        }
    }

	fColour = vec4(color, 1);
}
