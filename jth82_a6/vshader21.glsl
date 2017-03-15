#version 150

in vec3 vPosition;
in vec3 vNormal;

out vec3 color;
out vec3 position;
out vec3 normal;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix, projectionMatrix, translateMatrix;
uniform vec3 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec3 cameraPosition;
uniform float m_shininess;
uniform float isGouraud;


vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void main()
{
	color = vNormal;
	normal = vNormal;
	position = (modelViewProjectionMatrix * vec4(vPosition,1.0)).xyz;
    gl_Position =  modelViewProjectionMatrix * vec4(vPosition,1.0);


/*
	mat4 Projection = projectionMatrix;
	mat4 ModelView = modelViewMatrix;
	vec3 LightPosition = l_position;
	
	if(isGouraud > .5f) {


		vec3 LightPosition = l_position;
		
		vec3 pos = (ModelView * vec4(vPosition,1.0)).xyz;
		 // Light defined in camera frame
		 vec3 L = normalize( LightPosition - pos );
		 vec3 E = normalize( -pos );
		 vec3 H = normalize( L + E );
		 // Transform vertex normal into eye coordinates
		 vec3 N = normalize( ModelView*vec4(vNormal,0.0) ).xyz; 
		 
		 
		 vec3 ambient = vProduct(l_ambient,m_reflect_ambient).xyz;
		  float dTerm = max( dot(L, N), 0.0 );
		  vec3 DiffuseProduct = vProduct(l_diffuse,m_reflect_diffuse).xyz;
		  vec3 diffuse = dTerm*DiffuseProduct;
		  float sTerm = pow( max(dot(N, H), 0.0), m_shininess );
		  vec3 SpecularProduct = vProduct(l_specular,m_reflect_specular).xyz;
		  vec3 specular = sTerm * SpecularProduct;
		  if( dot(L, N) < 0.0 ) {
			  specular = vec3(0.0, 0.0, 0.0);
		  }
		  gl_Position = Projection * ModelView * vec4(vPosition,1.0);
		  color = ambient + diffuse + specular; 
		  position = vPosition;
		  normal = vNormal;
	}
	else {
		gl_Position = Projection * ModelView * vec4(vPosition,1.0);
		position = vPosition;
		normal = vNormal;
	}
*/	
}
