/*
 * bezierCurve.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: josephheenan
 */

#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

typedef Angel::vec4 point3;

bool debug = true;

const int NumPoints = 16;
vec4 points[NumPoints];
vec4 normals[NumPoints];

/*
vec3 controlPoints[4] = {
        vec3( -.40, -.40, 0.0), vec3( -.20, .40, 0.0),
        vec3(.20, -.40, 0.0), vec3(.40, .40, 0.0) };
*/
GLuint Projection;


vec3 controlPoints[16] = {
		vec3(0.0, 0.0, 0.0),
		vec3(2.0, 0.0, 1.5),
		vec3(4.0, 0.0, 2.9 ),
		vec3(6.0, 0.0, 0.0 ),
		vec3(0.0, 2.0, 1.1 ),
		vec3(2.0, 2.0, 3.9 ),
		vec3(4.0, 2.0, 3.1 ),
		vec3(6.0, 2.0, 0.7 ),
		vec3(0.0, 4.0, -0.5),
		vec3(2.0, 4.0, 2.6 ),
		vec3(4.0, 4.0, 2.4 ),
		vec3(6.0, 4.0, 0.4 ),
		vec3(0.0, 6.0, 0.3 ),
		vec3(2.0, 6.0, -1.1),
		vec3(4.0, 6.0, 1.3 ),
		vec3(6.0, 6.0, -0.2)
};


color4 L_ambient = vec4(1.0,1,0.1,1);
color4 L_diffuse = vec4(1.0,1,0.1,1);
color4 L_specular = vec4(1.0,1.0,1.0,1);

color4 M_reflect_ambient = vec4(1.0,1,1.0,1.0);
color4 M_reflect_diffuse = vec4(1.0,1,1.0,1.0);
color4 M_reflect_specular = vec4(1.0,1,1.0,1.0);

float M_shininess = 10;

GLuint l_ambient, l_diffuse, l_specular, l_position, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, m_shininess;
GLuint cameraPosition;
GLuint isGouraud;

// Projection matrix : 45° Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
bool isPerspective = true;

// For Ortho coordinates
GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;

float IsGouraud = .6; // >.5 is true, otherwise false



void printVector(vec3 vIn) {
	printf("(%f, %f, %f)\n",vIn.x,vIn.y,vIn.z);
}


void printVector(vec4 vIn) {
	printf("(%f, %f, %f)\n",vIn.x,vIn.y,vIn.z);
}

void
keyboard( unsigned char key, int x, int y )
{
	switch(key) {
		case 'q':
			exit(0);
			break;
	}
}

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat  left = -4.0, right = 4.0;
    GLfloat  bottom = -3.0, top = 5.0;
    GLfloat  zNear = -10.0, zFar = 10.0;

    GLfloat  aspect = GLfloat(width)/height;

    if ( aspect > 0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }

    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

void
initMainWindow( void )
{


    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );




    // Draw Bezier Patch; each has 16 vertices



    float N = NumPoints;
    float d = 1.0/(N-1.0);
    float u, uu;
    for(int i = 0; i < N; i++) {




    	u = i*d;
    	uu = 1.0-u;

    	printf("Control point x, y is: %f,%f\n",controlPoints[1].x,controlPoints[1].y);

		points[i].x = controlPoints[0].x*uu*uu*uu
				+ 3.0*controlPoints[1].x*uu*uu*u
				+ 3.0*controlPoints[2].x*uu*u*u
				+ 3.0*controlPoints[3].x*u*u*u;

		points[i].y = controlPoints[0].y*uu*uu*uu
				+ 3.0*controlPoints[1].y*uu*uu*u
				+ 3.0*controlPoints[2].y*uu*u*u
				+ 3.0*controlPoints[3].y*u*u*u;

		printf("(Setting point: x is %f, y is %f)\n",points[i].x,points[i].y);

    }

    // Print points and normals info
	if(debug) {
		for(int i = 0; i < NumPoints; i++) {
			vec4 currentPoint = points[i];
			printf("(Point)");
			printVector(currentPoint);
		}
	}

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshaderBezier.glsl", "fShaderBezier.glsl" );

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_TRUE, 0,
                           BUFFER_OFFSET(sizeof(normals)) );



    // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    Projection = glGetUniformLocation( program, "Projection" );

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background


}

void
displayMainWindow( void )
{

   glClear( GL_COLOR_BUFFER_BIT);
   glDrawArrays( GL_TRIANGLES, 0, NumPoints );

   glutSwapBuffers();

}




int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
#endif
    glutInitWindowSize( 500, 500 );

    glutCreateWindow( "Bezier Curve" );
#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif



	std::cout << "Press: x - To exit the program" << std::endl;


	initMainWindow();

	glutDisplayFunc( displayMainWindow );
    glutReshapeFunc( reshape );

	glutKeyboardFunc( keyboard );

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}



