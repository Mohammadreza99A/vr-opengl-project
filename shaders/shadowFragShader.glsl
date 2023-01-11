#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec2 tex_vcoord;
in vec3 v_frag_coord; 
in vec3 v_normal; 

uniform vec3 u_view_pos; 

uniform sampler2D shadow_buffer_tex;
in vec4 shadow_coord;

uniform uint shadow_buffer_tex_size;

float get_shadow_val(float offset_x, float offset_y);

// light
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

uniform Light light;

uniform float shininess; 
uniform vec3 materialColour; 
uniform sampler2D f_texture;


float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
    vec3 R = reflect (-L,N);    //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
    float cosTheta = dot(R , V); 
    float spec = pow(max(cosTheta,0.0), 32.0); 
    return light.specular_strength * spec;
}

void main() { 
    float shadow = 1;
    shadow = get_shadow_val(0.0, 0.0);

    vec3 N = normalize(v_normal);
    vec3 L = normalize(light.light_pos - v_frag_coord) ; 
    vec3 V = normalize(u_view_pos - v_frag_coord); 
    
    float specular = specularCalculation( N, L, V); 
    float diffuse = light.diffuse_strength * max(dot(N,L),0.0);
    float distance = length(light.light_pos - v_frag_coord);
    // float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    float light = light.ambient_strength + (diffuse + specular); 
    vec4 frag_light = vec4(materialColour * vec3(light), 1.0);
    FragColor =  texture(f_texture, tex_vcoord) * frag_light * shadow;
} 

float get_shadow_val(float offset_x, float offset_y){

   float shadow = 1.0;
   vec2 offset = vec2(offset_x, offset_y);

   float BIAS = 0.002;

   if ( texture( shadow_buffer_tex, (shadow_coord.xy/shadow_coord.w)+offset ).x  <  shadow_coord.z/shadow_coord.w-BIAS){
       shadow = 0.0;
   }
   return shadow;
}