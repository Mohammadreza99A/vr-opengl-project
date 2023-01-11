#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec2 tex_vcoord;

in vec3 surface_normal;
in vec3 to_light;
in vec3 to_camera;

uniform vec3 u_view_pos; 

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
uniform sampler2D background_texture;

uniform sampler2D r_texture;
uniform sampler2D g_texture;
uniform sampler2D b_texture;
uniform sampler2D blendmap;


float specularCalculation(vec3 N, vec3 L, vec3 V ){ 
    vec3 R = reflect (-L,N);    //reflect (-L,N) is  equivalent to //max (2 * dot(N,L) * N - L , 0.0) ;
    float cosTheta = dot(R , V); 
    float spec = pow(max(cosTheta,0.0), 32.0); 
    return light.specular_strength * spec;
}

void main() { 

    vec4 blendmap_texture_col = texture(blendmap,tex_vcoord );
    float back_texture=1-(blendmap_texture_col.r+blendmap_texture_col.g+blendmap_texture_col.b);
    vec2 tile_coords=tex_vcoord*70.0;

    vec4 background_texture_col = texture(background_texture,tile_coords ) * back_texture;

    vec4 r_texture_col= texture(r_texture,tile_coords ) * blendmap_texture_col.r;
    vec4 g_texture_col= texture(g_texture,tile_coords ) * blendmap_texture_col.g;
    vec4 b_texture_col= texture(b_texture,tile_coords ) * blendmap_texture_col.b;

    vec4 total_texture= background_texture_col + r_texture_col + g_texture_col+ b_texture_col;
    
    vec3 N = normalize(surface_normal);
	vec3 L = normalize(to_light);

	vec3 V = normalize(to_camera);

    
    float specular = specularCalculation( N, L, V); 
    float diffuse = light.diffuse_strength * max(dot(N,L),0.2);
    float light = light.ambient_strength + (diffuse + specular); 
    vec4 frag_light = vec4(materialColour * vec3(light), 1.0);
    FragColor =  total_texture * frag_light;
} 
