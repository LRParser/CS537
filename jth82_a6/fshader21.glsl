#version 150

 
 // per-fragment interpolated values from the vertex shader 
in vec3 fN; 
in vec3 fL; 
in vec3 fE; 
in vec3 normal;
out vec4 frag_color; 
uniform vec3 AmbientProduct, DiffuseProduct, SpecularProduct; 
uniform mat4 ModelView; 
uniform float Shininess; 

void main()  
{  
    // Normalize the input lighting vectors 
   vec3 N = normalize(fN); 
    vec3 E = normalize(fE); 
    vec3 L = normalize(fL); 
    vec3 H = normalize( L + E );    
    vec3 ambient = AmbientProduct; 
    
        float dTerm = max(dot(L, N), 0.0); 
    vec3 diffuse = dTerm*DiffuseProduct; 
    float sTerm = pow(max(dot(N, H), 0.0), Shininess); 
    vec3 specular = sTerm*SpecularProduct; 
    // discard the specular highlight if the light's behind the vertex 
    if( dot(L, N) < 0.0 )  
		specular = vec3(0.0, 0.0, 0.0); 
    // frag_color = vec4(ambient + diffuse + specular, 1.0); 
    frag_color = vec4(normal, 1.0); 
    
}  