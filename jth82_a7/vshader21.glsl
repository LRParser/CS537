#version 150

in vec4 vPosition;
in vec4 vNormal;

out vec4 color;


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

	// Computed ambient, diffuse and specular colors
	vec4 c_ambient = vProduct(l_ambient,m_reflect_ambient);
	
	vec4 L = l_position - vPosition;

	float d = dot(vNormal, normalize(L));

	vec4 c_diffuse;
	if(d > 0) {
		c_diffuse = vProduct(l_diffuse, m_reflect_diffuse)*d;
	}
	else {
		c_diffuse = vec4(0,0,0,1);
	}
	
	position = vPosition;
	color = c_ambient + c_diffuse;

	gl_Position =  transformMatrix * vPosition;
}
