// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <math.h>

const int NumPoints = 100;

//--------------------------------------------------------------------------

vec3 circleVertices[NumPoints];
vec3 colors[NumPoints];

void
init( void )
{

	// Specify the vertices for a circle
	// Increment from 0 to 2*Pi
	double twicePi = 2 * M_PI;


	double angle = 0;
	int i = 0;
	while(angle < twicePi && i < NumPoints) {
		float x = cos(angle) * .5;
		float y = sin(angle) *.5;
		printf("x is : %f, y is: %f \n",x,y);
		circleVertices[i] = vec3(x,y,0.0);
		colors[i] = vec3(angle / twicePi,0.0,0.0);
		printf("angle is: %f, color1 is : %f\n",angle, colors[i].x);
		i++;
		angle += 0.0628;
	}

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(circleVertices) + sizeof(colors), NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(circleVertices), circleVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(circleVertices), sizeof(colors), colors );


	// Set vertex color metadata


    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    //  glUseProgram( program );  // This is called in InitShader

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(circleVertices)) );

    glEnable( GL_DEPTH_TEST );


    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_TRIANGLE_FAN, 0, NumPoints );    // draw the points
    glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 'q':
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE);
#endif     
    glutInitWindowSize( 512, 512 );

    glutCreateWindow( "Red Triangle" );

#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif

    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}
