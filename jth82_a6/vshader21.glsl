#version 130

in vec4 vPosition;
in vec4 vNormal;

out vec4 color;

uniform mat4 transformMatrix;
uniform vec4 l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular;


void main()
{

        color = vec4(0.5,0,0,0);
        gl_Position =  transformMatrix * vPosition;
}
