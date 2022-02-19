#version 420 core
#define NBLamp 3

in vec3 Normal;
in vec3 FragPos;
in vec2 TextCoord;
out vec4 FragColor;
//Light obj
uniform vec3 lightColor[NBLamp];
uniform vec3 lightPos[NBLamp];
uniform float range[NBLamp];
uniform float strength[NBLamp];

//Spotlight
uniform vec3 spotDir[NBLamp]; //Direction du spot
uniform float cutOff[NBLamp]; //Angle d'éclairage
uniform float outerCutOff[NBLamp];
uniform bool isSpot[NBLamp];


uniform vec3 viewPos;
uniform sampler2D ourTexture;

void main()
{
    float ambientStrength = 0.01f;
    vec4 ambient = ambientStrength * vec4(1.0,1.0,1.0,1.0);
    vec3 result = vec3(0.0);
    vec3 norm;
    vec3 lightDir;
    float dist;

    float diff;
    vec3 diffuse;
    float specularStrength;
    vec3 viewDir;
    vec3 reflectDir;
    float spec;
    vec3 specular;

    float theta;
    float epsilon;
    float intensity;

    for(int i = 0; i < NBLamp; i++){
        norm = normalize(Normal);
        lightDir = normalize(lightPos[i] - FragPos);
        dist = abs(distance(lightPos[i], FragPos));


        theta = dot(lightDir, normalize(-spotDir[i]));
        epsilon = cutOff[i] - outerCutOff[i];
        intensity = clamp((theta - outerCutOff[i]) / epsilon, 0.0, 1.0);

        if(theta > cutOff[i] && isSpot[i]){      
            diff = max(dot(norm, lightDir), 0.0);
            diffuse = 0.3f * diff * lightColor[i];

            specularStrength = 0.2;
            viewDir = normalize(viewPos - FragPos);
            reflectDir = reflect(-lightDir, norm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
            specular = specularStrength * spec * lightColor[i];

            result += strength[i] * (1 - intensity) * min((range[i]/dist),1.0) * (diffuse + specular);
        }
        if(!isSpot[i]){
            diff = max(dot(norm, lightDir), 0.0);
            diffuse = 0.3f * diff * lightColor[i];

            specularStrength = 0.2;
            viewDir = normalize(viewPos - FragPos);
            reflectDir = reflect(-lightDir, norm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
            specular = specularStrength * spec * lightColor[i];

            result += strength[i] * min((range[i]/dist),1.0) * (diffuse + specular);
        }

        
    }    
    FragColor = (vec4(result, 1.0) + ambient) * texture(ourTexture, TextCoord);
}