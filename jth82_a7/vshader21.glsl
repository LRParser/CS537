#version 150

in vec3 vPosition;
in vec3 vNormal;

out vec3 vPos;
out vec3 vNorm;


uniform mat4 modelMatrix, viewMatrix, projectionMatrix, transformMatrix;
uniform vec3 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 eyePosition;
uniform float m_shininess;

vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void main()
{

	vPos = vPosition;
	vNorm = vNormal;
	gl_Position =  transformMatrix * vec4(vPosition,1.0);

}
