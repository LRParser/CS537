#version 150

in vec4 vPosition;
in vec4 vNormal;

out vec4 color;
out vec4 position;
out vec4 normal;

uniform mat4 transformMatrix;
uniform vec4 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 cameraPosition;
uniform float m_shininess;

uniform float shade1Solid, shade2Solid, shade3Solid;

vec4 vProduct(vec4 a, vec4 b) {
	return vec4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]);
}

void main()
{

	// Computed ambient, diffuse and specular colors
	vec4 c_ambient = vProduct(l_ambient,m_reflect_ambient);

	float d = dot(vNormal, normalize(l_position));

	vec4 c_diffuse;
	if(d > 0) {
		c_diffuse = vProduct(l_diffuse, m_reflect_diffuse)*d;
	}
	else {
		c_diffuse = vec4(0.2,0.2,0.2,1);
	}

	vec4 viewDirection = (transformMatrix * vPosition) - cameraPosition;
	vec4 halfVector = normalize(l_position + viewDirection);

	vec4 c_specular = vec4(0,0,0,0);
	float s = dot(halfVector,vNormal);
	if(s >0.0) {
		c_specular = pow(s,m_shininess) * vProduct(l_specular,m_reflect_specular);
	}
	else {
		c_specular = -1 * pow(s,m_shininess) * vProduct(l_specular,m_reflect_specular);
	}

	position = vPosition;
	normal = vNormal;

	color = c_ambient + c_diffuse + c_specular;
	
	if(shade1Solid > .4) {
		color = vec4(1,0,0,1);
	}
	else if(shade2Solid > .4) {
		color = vec4(0,1,0,1);
	}
	else if(shade3Solid > .4) {
		color = vec4(0,0,1,1);
	}
	
	gl_Position =  transformMatrix * vPosition;
}
