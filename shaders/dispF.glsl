#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    mat3 TBN;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform float shininess; 
uniform sampler2D depthMap;

uniform float heightScale;



struct Light{ 
    vec3 light_pos; 
    float ambient_strength; 
    float diffuse_strength; 
    float specular_strength; 
    //attenuation factor
    float constant;
    float linear;
    float quadratic;
    };
uniform Light light;

uniform vec3 viewPos;


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(depthMap, texCoords).r;     
    return texCoords - viewDir.xy * (height * heightScale);   
} 

float specularCalculation(vec3 N, vec3 L, vec3 V, float shininess ){ 
			vec3 R = reflect (-L,N);    //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
			float cosTheta = dot(R , V); 
			float spec = pow(max(cosTheta,0.0), shininess); 
			return light.specular_strength * spec;
		}

void main()
{          

    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);       
    //if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    //    discard;      

    // obtain normal from normal map in range [0,1]
   vec3 normal = texture(normalMap, texCoords).rgb;
   // transform normal vector to range [-1,1]
   normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space



   // get diffuse color
   vec3 color = texture(diffuseMap, texCoords).rgb;


   
   // ambient

   vec3 ambient = light.ambient_strength * color;

   // diffuse
   vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
   float diff =  max(dot(lightDir, normal), light.diffuse_strength);
   vec3 diffuse = diff * color;

   // specular 
   float spec = specularCalculation( normal, lightDir, viewDir, shininess); 
   vec3 specular = vec3(0.2) * spec;


    float distance = length(light.light_pos - fs_in.FragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

   FragColor = vec4(ambient + diffuse + specular + attenuation, 1.0);
}