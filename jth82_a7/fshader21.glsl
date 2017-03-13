#version 150

in vec3 vPos;
in vec3 vNorm;

out vec4 FragColor;

uniform mat4 modelMatrix, viewMatrix, projectionMatrix, transformMatrix;
uniform vec3 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, l_position;
uniform vec4 eyePosition;
uniform float m_shininess;

vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void main()
{
	
	// Computed ambient, diffuse and specular colors
    vec3 N = vNorm;
    vec3 E = vec3(eyePosition.x,eyePosition.y,eyePosition.z) - vPos;

    // N and E point in the "same" direction, i.e. the normal points away from the camera
    if (dot(N, E) > 0)  {
      N = -1 * N;
    }
	
	vec3 c_ambient = vProduct(l_ambient,m_reflect_ambient);
	
	vec3 L = l_position - vPos;

	float d = dot(N, normalize(L));

	vec3 c_diffuse;
	if(d > 0) {
		c_diffuse = vProduct(l_diffuse, m_reflect_diffuse)*d;
	}
	else {
		c_diffuse = vec3(0,0,0);
	}
	
	vec3 color = c_ambient + c_diffuse;

	FragColor = vec4(color,1.0);
}
