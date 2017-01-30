// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <math.h>

const int ellipsePoints = 100;
const int pointsPerSquare = 12;
const int numSquares = 6;
const int squarePoints = pointsPerSquare * numSquares;

const int NumPoints = ellipsePoints + squarePoints;
const double TwicePi = 2 * M_PI;
const int squaresStartIdx = 0;

typedef struct window_info window_info;
struct window_info {
	GLuint vao[1];
	GLuint buffer;
};

window_info subWindowInfo;

//--------------------------------------------------------------------------

vec3 vertices[NumPoints];
vec3 colors[NumPoints];

vec3 subVertices[NumPoints];
vec3 subColors[NumPoints];
int mainWindow, subWindow1;

int w = 500;
int h = 500;
int border = 50;

GLint windowHeight, windowWidth;

// Function can only be called twice
void createCircle(vec3 buffer[], vec3 colors[], float baseScaleFactor, int startIndex, int endIndex,
		float scalingFactor, bool doShading, float xOffset, float yOffset) {
	double angle = 0;
	int i = startIndex;
	while(angle < TwicePi && i < endIndex) {
		float x = (cos(angle) * baseScaleFactor) - xOffset;
		float y = (sin(angle) * baseScaleFactor * scalingFactor) - yOffset;
		buffer[i] = vec3(x,y,0.0);
		if(doShading) {
			colors[i] = vec3(angle / TwicePi,0.0,0.0);
		}
		else {
			colors[i] = vec3(1.0,0.0,0.0);
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
initMainWindow( void )
{

	// Shaded circle
	createCircle(vertices,colors,.3,0,100,1.2,true,-0.6,-0.7);

	/*
	// Ellipse
	createCircle(vertices,colors,.3,100,200,0.2,false,0.6,-0.7);

	// Colored square
	float triangleXOffset = 0.6;
	vertices[200] = vec3(0.35 - triangleXOffset,.6,0.0);
	colors[200] = vec3(0.0,1.0,0.0);
	vertices[201] = vec3(0.35 - triangleXOffset,1.0,0.0);
	colors[201] = vec3(1.0,0.0,0.0);
	vertices[202] = vec3(0.75 - triangleXOffset,.6,0.0);
	colors[202] = vec3(0.0,0.0,1.0);
	vertices[203] = vec3(0.75 - triangleXOffset,.6,0.0);
	colors[203] = vec3(0.0,0.0,1.0);

	// Squares, draw white, black, white, black, white, black


	float overallScale = .5;
	createSquare(204,overallScale,0,0,.7,.7,true,0.0);
	createSquare(204 + 1 * pointsPerSquare,overallScale,0,0,.6,.6,false,0.1);
	createSquare(204 + 2 * pointsPerSquare,overallScale,0,0,.5,.5,true,0.2);
	createSquare(204 + 3 * pointsPerSquare,overallScale,0,0,.4,.4,false,0.3);
	createSquare(204 + 4 * pointsPerSquare,overallScale,0,0,.3,.3,true,0.4);
	createSquare(204 + 5 * pointsPerSquare,overallScale,0,0,.2,.2,false,0.5);
*/

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

    glEnable( GL_DEPTH_TEST );


    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background

	/*
	// Squares, draw white, black, white, black, white, black

	float overallScale = .5;

	createSquare(squaresStartIdx,overallScale,0,0,.7,.7,true,0.0);
	createSquare(squaresStartIdx + 1 * pointsPerSquare,overallScale,0,0,.6,.6,false,0.1);
	createSquare(squaresStartIdx + 2 * pointsPerSquare,overallScale,0,0,.5,.5,true,0.2);
	createSquare(squaresStartIdx + 3 * pointsPerSquare,overallScale,0,0,.4,.4,false,0.3);
	createSquare(squaresStartIdx + 4 * pointsPerSquare,overallScale,0,0,.3,.3,true,0.4);
	createSquare(squaresStartIdx + 5 * pointsPerSquare,overallScale,0,0,.2,.2,false,0.5);

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
    // glUseProgram( program );  // This is called in InitShader

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(vertices)) );
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // black background
    */

}

void
displayMainWindow( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glDrawArrays( GL_TRIANGLE_FAN, 0, 100 );    // draw the shaded circle
    glFlush();

}


//----------------------------------------------------------------------------

void initSubWindow(void) {

	createCircle(subVertices,subColors,0,0,0,0.2,false,0.6,-0.7);

    glGenVertexArrays( 1, subWindowInfo.vao );
    glBindVertexArray( subWindowInfo.vao[0] );

    // Create and initialize a buffer object

    glGenBuffers( 1, &subWindowInfo.buffer );
    glBindBuffer( GL_ARRAY_BUFFER, subWindowInfo.buffer );

    size_t totalSize = sizeof(subVertices) +
    		sizeof(subColors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(subVertices),
    		subVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(subVertices), sizeof(subColors),
    		subColors );


    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    //glUseProgram( program );  // This is called in InitShader

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(vertices)) );
    glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // blue background
}

void displaySubWindow() {

    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

	createCircle(subVertices,subColors,0,0,0,0.2,false,0.6,-0.7);

    glGenVertexArrays( 1, subWindowInfo.vao );
    glBindVertexArray( subWindowInfo.vao[0] );

    // Create and initialize a buffer object

    glGenBuffers( 1, &subWindowInfo.buffer );
    glBindBuffer( GL_ARRAY_BUFFER, subWindowInfo.buffer );

    size_t totalSize = sizeof(subVertices) +
    		sizeof(subColors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(subVertices),
    		subVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(subVertices), sizeof(subColors),
    		subColors );


	glutSetWindow(subWindow1);
    glClearColor( 0.0, 0.0, 1.0, 1.0 ); // blue background

	glClear( GL_COLOR_BUFFER_BIT );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 99 );    // draw the shaded circle
    glFlush();
    glutSwapBuffers();
}


//----------------------------------------------------------------------------

void reshape(GLsizei w, GLsizei h) {
	windowWidth = w;
	windowHeight = h;
	glViewport(0,0,windowWidth,windowHeight);
}

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 'q':
        exit( EXIT_SUCCESS );
        break;
    }
}

void idle() {

	glutPostRedisplay();
}

//----------------------------------------------------------------------------



int
main( int argc, char **argv )
{




	glutInit( &argc, argv );
	#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA );
	#else
	    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	#endif
	#ifndef __APPLE__
	    GLenum err = glewInit();
	    if (GLEW_OK != err)
	      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	#endif

	    glutInitWindowSize( w, h );

	    mainWindow = glutCreateWindow( "Assignment 3" );
	    initMainWindow();
	    glutDisplayFunc( displayMainWindow );
	    glutKeyboardFunc( keyboard );

	    /*
	    initSubWindow();
	    subWindow1 = glutCreateSubWindow(mainWindow,
	     0,0,100, 100);
	     // Must register a display func for each window
	     glutDisplayFunc(displaySubWindow);
*/
		glutIdleFunc(idle);
	    glutMainLoop();
	    return 0;
}
