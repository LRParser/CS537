// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <math.h>

const int shadedCirclePoints = 100;
const int ellipsePoints = 100;
const int coloredTrianglePoints = 3;
const int pointsPerSquare = 12;
const int numSquares = 6;
const int squarePoints = pointsPerSquare * numSquares;

const int NumPoints = shadedCirclePoints + ellipsePoints + coloredTrianglePoints + squarePoints;
const double TwicePi = 2 * M_PI;


//--------------------------------------------------------------------------

vec3 vertices[NumPoints];
vec3 colors[NumPoints];


// Function can only be called twice
void createCircle(vec3 buffer[], vec3 colors[], float baseScaleFactor, int startIndex, int endIndex,
		float scalingFactor, bool doShading, float xOffset, float yOffset) {
	double angle = 0;
	int i = startIndex;
	while(angle < TwicePi && i < endIndex) {
		float x = (cos(angle) * baseScaleFactor) - xOffset;
		float y = (sin(angle) * baseScaleFactor * scalingFactor) - yOffset;
		printf("x is : %f, y is: %f \n",x,y);
		buffer[i] = vec3(x,y,0.0);
		if(doShading) {
			colors[i] = vec3(angle / TwicePi,0.0,0.0);
			printf("angle is: %f, color1 is : %f\n",angle, colors[i].x);

		}
		else {
			colors[i] = vec3(1.0,0.0,0.0);
			printf("angle is: %f, color1 is : %f\n",angle, 1.0);

		}
		i++;
		angle += 0.0628;
	}
}

void createSquare(int startVertex, float scaleFactor, float centroidX, float centroidY, float xScale, float yScale, bool isWhite, float zIndex) {

	vec3 whiteColor = vec3(1.0,1.0,1.0);
	vec3 blackColor = vec3(0.0,0.0,0.0);
	vec3 color = isWhite ? whiteColor : blackColor;

	int currentVertex = startVertex;

	// Make square from 4 triangles all "pointing" towards center point

	// Triangle 1
	vertices[currentVertex] = vec3(centroidX,centroidY,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	float normalizedXScale = scaleFactor * xScale;
	float normalizedYScale = scaleFactor * yScale;

	vertices[currentVertex] = vec3(centroidX - normalizedXScale,centroidY - normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	vertices[currentVertex] = vec3(centroidX + normalizedXScale,centroidY - normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	// Triangle 2, center + 2 points
	vertices[currentVertex] = vec3(centroidX,centroidY,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	vertices[currentVertex] = vec3(centroidX + normalizedXScale,centroidY - normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	vertices[currentVertex] = vec3(centroidX + normalizedXScale,centroidY + normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	// Triangle 3, center + 2 points
	vertices[currentVertex] = vec3(centroidX,centroidY,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	vertices[currentVertex] = vec3(centroidX - normalizedXScale,centroidY + normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	vertices[currentVertex] = vec3(centroidX + normalizedXScale,centroidY + normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	// Triangle 4, center + 2 points

	vertices[currentVertex] = vec3(centroidX,centroidY,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;


	vertices[currentVertex] = vec3(centroidX - normalizedXScale,centroidY + normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	vertices[currentVertex] = vec3(centroidX - normalizedXScale,centroidY - normalizedYScale,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;
}

void
init( void )
{

	// Shaded circle
	createCircle(vertices,colors,.3,0,100,1,true,-0.6,-0.7);

	// Ellipse
	createCircle(vertices,colors,.3,100,200,0.6,false,0.6,-0.7);

	// Colored triangle
	float triangleXOffset = 0.6;
	vertices[200] = vec3(0.35 - triangleXOffset,.6,0.0);
	colors[200] = vec3(0.0,1.0,0.0);
	vertices[201] = vec3(0.55 - triangleXOffset,1.0,0.0);
	colors[201] = vec3(1.0,0.0,0.0);
	vertices[202] = vec3(0.75 - triangleXOffset,.6,0.0);
	colors[202] = vec3(0.0,0.0,1.0);

	// Squares, draw white, black, white, black, white, black


	float overallScale = .5;
	createSquare(203,overallScale,0,0,.7,.7,true,0.0);
	createSquare(203 + 1 * pointsPerSquare,overallScale,0,0,.6,.6,false,0.1);
	createSquare(203 + 2 * pointsPerSquare,overallScale,0,0,.5,.5,true,0.2);
	createSquare(203 + 3 * pointsPerSquare,overallScale,0,0,.4,.4,false,0.3);
	createSquare(203 + 4 * pointsPerSquare,overallScale,0,0,.3,.3,true,0.4);
	createSquare(203 + 5 * pointsPerSquare,overallScale,0,0,.2,.2,false,0.5);


	/*
	// Squares, draw black, white, black, white, black, white
	float overallScale = .5;
		createSquare(203,overallScale,0,0,.2,.2,false,.9);

		createSquare(203 + 1 * pointsPerSquare,overallScale,0,0,.3,.3,true,-.9);
		createSquare(203 + 2 * pointsPerSquare,overallScale,0,0,.4,.4,false,0.8);
		createSquare(203 + 3 * pointsPerSquare,overallScale,0,0,.5,.5,true,0.7);
		createSquare(203 + 4 * pointsPerSquare,overallScale,0,0,.6,.6,false,0.6);
		createSquare(203 + 5 * pointsPerSquare,overallScale,0,0,.7,.7,true,0.5);
*/

	// Now we need to make 3 squares, each smaller than the other







    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );
    

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    size_t totalSize = sizeof(vertices) +
    		sizeof(colors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertices),
    		vertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors),
    		colors );


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
                           BUFFER_OFFSET(sizeof(vertices)) );

    printf("vColor is set\n");

    glEnable( GL_DEPTH_TEST );


    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glDrawArrays( GL_TRIANGLE_FAN, 0, 100 );    // draw the shaded circle
    glFlush();

    glDrawArrays( GL_TRIANGLE_FAN, 100, 100 );    // draw the ellipse
    glFlush();

    glDrawArrays(GL_TRIANGLES,200,3);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203,pointsPerSquare);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203 + 1 * pointsPerSquare,pointsPerSquare);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203 + 2 * pointsPerSquare,pointsPerSquare);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203 + 3 * pointsPerSquare,pointsPerSquare);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203 + 4 * pointsPerSquare,pointsPerSquare);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203 + 5 * pointsPerSquare,pointsPerSquare);
    glFlush();

    glDrawArrays(GL_TRIANGLES,203 + 6 * pointsPerSquare,pointsPerSquare);
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
    glutInitWindowSize( 500, 500 );

    glutCreateWindow( "Assignment 2" );

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
