#version 150

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord; //texture coordinate from application  

out vec3 vPos;
out vec3 vNorm;
out vec2 texCoord; //output tex coordinate to be interpolated 


uniform mat4 modelMatrix, viewMatrix, projectionMatrix, transformMatrix;
uniform vec3 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 eyePosition;
uniform float m_shininess;
uniform sampler2D texture; //texture object id from application 


vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void main()
{

	vPos = vPosition;
	vNorm = vNormal;
	texCoord = vTexCoord;
	gl_Position =  transformMatrix * vec4(vPosition,1.0);

}
