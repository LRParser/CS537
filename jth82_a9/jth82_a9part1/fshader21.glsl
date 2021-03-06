#version 150

 // per-fragment interpolated values from the vertex shader 
in vec3 fN; 
in vec3 fL; 
in vec3 fE; 
in vec3 normal;
in vec2 texCoord;
out vec4 frag_color; 
uniform vec3 AmbientProduct, DiffuseProduct, SpecularProduct; 
uniform mat4 ModelView; 
uniform float Shininess; 
uniform sampler2D modelTexture; //texture object id from application 

void main()  
{  
    // Normalize the input lighting vectors 
   	vec3 N = normalize(fN); 
    vec3 E = normalize(fE); 
    vec3 L = normalize(fL); 
    vec3 H = normalize( L + E );    
    vec3 ambient = AmbientProduct; 

    float sTerm = pow(max(dot(N, H), 0.0), Shininess); 
    vec3 specular = sTerm*SpecularProduct; 
    // discard the specular highlight if the light's behind the vertex 
    if( dot(L, N) < 0.0 )  
		specular = vec3(0.0, 0.0, 0.0);  
		
	
	float dTerm = max(dot(L, N), 0.0); // When using this, back isn't shaded
    vec3 diffuse = texture( modelTexture, texCoord ).xyz; 	
	// Replace diffuse color by a texture
	// diffuse =   
    frag_color = vec4(ambient + diffuse + specular, 1.0); 
    
}  