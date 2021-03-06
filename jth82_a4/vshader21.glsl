#version 150

in vec4 vPosition;
in vec3 vColor;
out vec4 color;

uniform mat4 transformMatrix;

void main()
{

		// Reference is Angel Chapter 3 Example 6
        color = vec4(vColor,1.0);
        gl_Position =  transformMatrix * vPosition;
}
