#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec2 tex_vcoord;
in vec3 v_frag_coord; 
in vec3 v_normal; 

uniform vec3 u_view_pos; 

// direction light ->sun
struct Light{ 

vec3 light_pos; 
float ambient_strength; 
float diffuse_strength; 
float specular_strength; 
//attenuation factor
//float constant;
//float linear;
//float quadratic;
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

uniform float shininess; 
uniform vec3 materialColour; 
uniform sampler2D f_texture;


float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
    vec3 R = reflect (-L,N);    //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
    float cosTheta = dot(R , V); 
    float spec = pow(max(cosTheta,0.0), 32.0); 
    return spec;
}

// calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, vec3 N , vec3 V)
{
    vec3 L = normalize(light.position - v_frag_coord) ; 
    float spec = specularCalculation( N, L, V); 
    float diff = light.diffuse * max(dot(N,L),0.0);
    float distance = length(light.position - v_frag_coord);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);    
    // combine results
    vec3 ambient = light.ambient * light.color * vec3(texture(f_texture, tex_vcoord));
    vec3 diffuse = light.diffuse * light.color * diff * vec3(texture(f_texture, tex_vcoord));
    vec3 specular = light.specular * light.color*spec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return vec4(ambient + diffuse + specular, 1.0);
}



// calculates the color when using a directional light.
vec4 CalcDirLight(Light light, vec3 N, vec3 V)
{
    vec3 L = normalize(light.light_pos - v_frag_coord) ; 
    float specular = light.specular_strength *specularCalculation( N, L, V); 
    float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
    float distance = length(light.light_pos - v_frag_coord);
    // float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    float light_value = light.ambient_strength + (diffuse + specular); 
    vec4 frag_light = vec4(materialColour * vec3(light_value), 1.0);
    return texture(f_texture, tex_vcoord) * frag_light;
}


void main() { 
    vec3 N = normalize(v_normal);
    vec3 V = normalize(u_view_pos - v_frag_coord); 

    vec4 result = CalcDirLight(light, N, V);
    
    FragColor =  result+CalcPointLight(point_light, N ,  V);
} 