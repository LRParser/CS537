#version 150

in vec4 color;
in vec4 position;
in vec4 normal;

out vec4 FragColor;


uniform mat4 modelViewMatrix, projectionMatrix;
uniform vec4 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 cameraPosition;
uniform float m_shininess;
uniform float isGouraud;


vec4 vProduct(vec4 a, vec4 b) {
	return vec4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]);
}

void main()
{
	mat4 Projection = projectionMatrix;
	mat4 ModelView = modelViewMatrix;
		
		if(isGouraud < .5f) {


			vec4 LightPosition = l_position;
			
			vec3 pos = (ModelView * position).xyz;
			 // Light defined in camera frame
			 vec3 L = normalize( LightPosition.xyz - pos );
			 vec3 E = normalize( -pos );
			 vec3 H = normalize( L + E );
			 // Transform vertex normal into eye coordinates
			 vec3 N = normalize( ModelView*normal ).xyz; 
			 
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
			  FragColor = vec4(ambient + diffuse + specular,1.0); 
		}
		else {
			FragColor = color;
		}		
}
