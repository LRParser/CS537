#version 150

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;

out vec3 fN; 
out vec3 fE; 
out vec3 fL;
out vec3 normal;
out vec2 texCoord;

uniform vec4 LightPosition; 
uniform vec3 EyePosition; 
uniform mat4 ModelView; 
uniform mat4 Projection; 

vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void main()
{

	fN = vNormal; 
    fE = EyePosition - vPosition.xyz; 
    //  Light defined in world coordinates 
    if ( LightPosition.w != 0.0 ) { 
		fL = LightPosition.xyz - vPosition.xyz; 
    } else { 
        fL = LightPosition.xyz; 
    } 
    
    texCoord = vTexCoord;
	normal = vec3(ModelView * vec4(vNormal,1.0)).xyz;
    gl_Position =  Projection * ModelView * vec4(vPosition,1.0);
}
