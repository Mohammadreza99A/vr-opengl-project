#version 330                                                                        
                                                                                                                                     
in vec3 position;
in vec3 normal;
uniform mat4 gWVP;                                                                  
                                                                                                                                             
                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = gWVP * vec4(position, 1.0);                                                    
}