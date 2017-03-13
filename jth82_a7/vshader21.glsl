#version 150

in vec3 vPosition;
in vec3 vNormal;

out vec3 color;


uniform mat4 transformMatrix;
uniform vec3 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 cameraPosition;
uniform float m_shininess;



vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void main()
{

	// Computed ambient, diffuse and specular colors
	vec3 c_ambient = vProduct(l_ambient,m_reflect_ambient);
	
	vec3 L = l_position - vPosition;

	float d = dot(vNormal, normalize(L));

	vec3 c_diffuse;
	if(d > 0) {
		c_diffuse = vProduct(l_diffuse, m_reflect_diffuse)*d;
	}
	else {
		c_diffuse = vec3(0,0,0);
	}
	
	color = c_ambient + c_diffuse;
	gl_Position =  transformMatrix * vec4(vPosition,1.0);
}
