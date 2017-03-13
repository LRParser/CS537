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


vec4 vProduct(vec4 a, vec4 b) {
	return vec4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]);
}

void main()
{
	    FragColor = color;
}
