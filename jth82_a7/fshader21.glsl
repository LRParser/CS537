#version 150

in vec4 color;
in vec4 position;
in vec4 normal;

out vec4 FragColor;


uniform mat4 transformMatrix;
uniform vec4 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 cameraPosition;
uniform float m_shininess;
uniform float isGouraud;
uniform float flatShading;


vec4 vProduct(vec4 a, vec4 b) {
	return vec4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]);
}

void main()
{

	
	if(isGouraud < .5f) {
		// Computed ambient, diffuse and specular colors
		vec4 c_ambient = vProduct(l_ambient,m_reflect_ambient);

		// If in Phong mode we set color to vNormal in vertex shader
		float d = dot(normal, normalize(l_position));

		vec4 c_diffuse;
		if(d > 0) {
			c_diffuse = vProduct(l_diffuse, m_reflect_diffuse)*d;
		}
		else {
			c_diffuse = vec4(0,0,0,1);
		}


		vec4 viewDirection = (transformMatrix * position) - cameraPosition;
		vec4 halfVector = normalize(l_position + viewDirection);

		vec4 c_specular = vec4(0,0,0,0);
		float s = dot(halfVector,normal);
		if(s >0.0) {
			c_specular = pow(s,m_shininess) * vProduct(l_specular,m_reflect_specular);
		}
		else {
			c_specular = vec4(0,0,0,0);
		}


		if(flatShading > 0.5) {
			FragColor = c_ambient + c_diffuse;
		}
		else {
			FragColor = c_ambient + c_diffuse + c_specular; // vec4(1.0,1.0,1.0,1.0); //
		}
		}
	else {
	    FragColor = color;

	}
	
	// FragColor = vec4(1.0,0,0,1);

}
