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


struct Light{ 
    vec3 light_pos; 
    float ambient_strength; 
    float diffuse_strength; 
    float specular_strength; 
    };
struct PointLight {
    vec3 position;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;
	
    float ambient;
    float diffuse;
    float specular;
};

uniform Light light;

uniform PointLight point_light;
uniform vec3 viewPos;


float specularCalculation(vec3 N, vec3 L, vec3 V, float shininess ){ 
			vec3 R = reflect (-L,N);    //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
			float cosTheta = dot(R , V); 
			float spec = pow(max(cosTheta,0.0), shininess); 
			return light.specular_strength * spec;
		}

vec4 CalcPointLight(PointLight light, vec3 N , vec3 V, vec3 color)
{

    vec3 ambient = light.ambient*color;
    vec3 pos=fs_in.TBN*light.position;
    vec3 L = normalize(fs_in.TangentFragPos-pos);
    vec3 specular = vec3(0.2) * specularCalculation( N, L, V,shininess); 
    vec3 diffuse = max(dot(L, N), light.diffuse)*color;
    float distance = length(pos - fs_in.TangentFragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);    
    ambient *=light.color;
    diffuse *= light.color;
    //specular *= attenuation;
    return vec4(ambient + diffuse + specular, 1.0);

}

// calculates the color when using a directional light.
vec4 CalcDirLight(Light light, vec3 N, vec3 V, vec3 color)
{

   vec3 ambient = light.ambient_strength * color;

    vec3 L = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 specular = vec3(0.2) * specularCalculation( N, L, V,shininess); 
    vec3 diffuse = max(dot(L, N), light.diffuse_strength) * color;
    return vec4(ambient + diffuse + specular, 1.0);
}


void main()
{           
    // obtain normal from normal map in range [0,1]
   vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
   // transform normal vector to range [-1,1]
   normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space



   // get diffuse color
   vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);


   vec4 res=CalcDirLight(light,normal, viewDir, color);

   FragColor =  res + CalcPointLight(point_light,normal, viewDir, color);
}