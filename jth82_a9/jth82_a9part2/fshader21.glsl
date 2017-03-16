#version 150

 
 // per-fragment interpolated values from the vertex shader  
in vec3 position;
in vec3 fN; 
in vec3 fL; 
in vec3 fE; 
in vec3 normal;
out vec4 frag_color; 
uniform vec3 AmbientProduct, DiffuseProduct, SpecularProduct; 
uniform mat4 ModelView; 
uniform float Shininess; 

/*
I would suggest that your mapping from 3D Cartesian space to RGB color space be based on a location

specified in world, NOT camera, coordinates. So you will need to pass the location of the vertex in world

coordinates to the fragment shader.  Write your procedural texture function in your fragment shader based

on the interpolated value of the vertex location.  Again, the color that you generate should be substituted

for the previously-constant material diffuse color in your Phong shading equation.

I'm thinking that for the 3D procedural texturing you will only need to change your vertex and fragment

shaders. I don't think that you will need to change any of your HW6 application code.
*/


vec3 textureValue(vec3 inputVec) {
	float x = inputVec.x;
	float y = inputVec.y;
	float z = inputVec.z;
	
	return vec3(x + cos(x),y + tan(y),z + sin(z));
	
}

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
	
	float dTerm = max(dot(L, N), 0.0); 
    vec3 diffuse = dTerm*DiffuseProduct; 	
    
    // Replace diffuse with a 3d texture
    diffuse = textureValue(position);
	
    frag_color = vec4(ambient + diffuse + specular, 1.0); 
    //frag_color = vec4(diffuse, 1.0); 
    
    
}  