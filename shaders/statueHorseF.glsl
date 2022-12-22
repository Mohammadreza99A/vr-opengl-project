#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec3 v_frag_coord; 
in vec3 v_normal; 

uniform vec3 u_view_pos; 

//In GLSL you can use structures to better organize your code


uniform vec3 material_colour; 
uniform samplerCube cubemap_sampler;
uniform float refractionIndice;
uniform float shininess; 
uniform vec3 light_color;

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


float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
    vec3 R = reflect (-L,N);    //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
    float cosTheta = dot(R , V); 
    float spec = pow(max(cosTheta,0.0), shininess); 
    return light.specular_strength * spec;
}


void main() { 
float ratio = 1.00 / refractionIndice;
vec3 N = normalize(v_normal);
vec3 L = normalize(light.light_pos - v_frag_coord) ; 
vec3 V = normalize(u_view_pos - v_frag_coord);

vec3 R = refract(-V,N,ratio); 
float specular = specularCalculation( N, L, V); 
float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
float distance = length(light.light_pos - v_frag_coord);
float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
float light = light.ambient_strength +attenuation* (diffuse + specular); 


vec3 color = light * material_colour *light_color;
FragColor = mix(texture(cubemap_sampler, R), vec4(color, 1.0), 0.4);

} 