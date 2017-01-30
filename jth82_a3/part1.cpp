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
const int squaresEndIdx = squaresStartIdx + 5 * pointsPerSquare;

typedef struct window_info window_info;
struct window_info {
	GLuint vao[1];
	GLuint buffer;
};

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

float blueValue = 1.0f;
float window2Red, window2Green, window2Blue = 0.0f;



window_info subWindowInfo;

//--------------------------------------------------------------------------

vec3 vertices[NumPoints];
vec3 colors[NumPoints];

vec3 subVertices[NumPoints];
vec3 subColors[NumPoints];

vec3 windowTwoVertices[NumPoints];
vec3 windowTwoColors[NumPoints];

int mainWindow, subWindow1, window2;

int w = 500;
int h = 500;
int border = 50;

GLint windowHeight, windowWidth;

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLuint  theta;  // The location of the "theta" shader uniform variable


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


    theta = glGetUniformLocation( program, "theta" );
    glEnable( GL_DEPTH_TEST );


    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background


}

void
displayMainWindow( void )
{
	glutSetWindow(mainWindow);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glUniform3fv( theta, 1, Theta );
    int totalSquareIdxs = pointsPerSquare * 6;

    glDrawArrays( GL_TRIANGLES, 0, totalSquareIdxs );


    glutSwapBuffers();

}


//----------------------------------------------------------------------------

void initSubWindow(void) {


	// Shaded circle
	createCircle(subVertices,subColors,.3,0,100,1.2,true,0,0);


    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );


    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    size_t totalSize = sizeof(subVertices) +
    		sizeof(subColors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(subVertices),
    		subVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(subVertices), sizeof(subColors),
    		subColors );


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


    glClearColor( 0.0, 1.0, blueValue, 0.0 ); // green background
}

void displaySubWindow() {

	glutSetWindow(subWindow1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glClearColor( 0.0, 1.0, blueValue, 0.0 ); // green background

    glDrawArrays( GL_TRIANGLE_FAN, 0, 100 );    // draw the shaded circle
    glFlush();
    glutSwapBuffers();
}

void initWindow2(void) {


	// Shaded circle
	createCircle(windowTwoVertices,windowTwoColors,.3,0,100,1.2,true,0,0);


    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );


    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    size_t totalSize = sizeof(subVertices) +
    		sizeof(subColors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(windowTwoVertices),
    		windowTwoVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(windowTwoVertices), sizeof(windowTwoColors),
    		windowTwoColors );


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


    glClearColor( window2Red, window2Green, window2Blue, 0.0 ); // green background
}

void displayWindow2() {

	glutSetWindow(window2);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glClearColor( window2Red, window2Green, window2Blue, 0.0 ); // green background

    glDrawArrays( GL_TRIANGLE_FAN, 0, 100 );    // draw the shaded circle
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

void
keyboardWindow2( unsigned char key, int x, int y )
{

	bool pressed = false;
    switch ( key ) {
		case 'q':
			exit( EXIT_SUCCESS );
			break;
		case 'r':
			pressed = true;
			window2Red = 1.0;
			break;
		case 'g':
			pressed = true;
			window2Green = 1.0;
			break;
		case 'b':
			pressed = true;
			window2Blue = 1.0;
			break;
    }
    if(pressed) {
        glutSetWindow(window2);
        glutPostRedisplay();
        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }


}

void idle() {
    Theta[Axis] += 0.05;

    if ( Theta[Axis] > 360.0 ) {
	Theta[Axis] -= 360.0;
    }
	glutPostRedisplay();
}

void idle2() {
    Theta[Axis] += 0.05;

    if ( Theta[Axis] > 360.0 ) {
	Theta[Axis] -= 360.0;
    }
	glutPostRedisplay();

}

//----------------------------------------------------------------------------

void menu_chooser(int id) {
	switch(id)
	{
	case 1:
		blueValue = 0.0f;

	    glClearColor( 0.0,1.0,blueValue,0.0f ); // green background
	    break;

	case 2:
		blueValue = 1.0f;

	    glClearColor( 0.0,1.0,blueValue,0.0f ); // green background
	    break;

	default:
		blueValue = 0.0f;
	    glClearColor( 0.0,1.0,blueValue,0.0f ); // green background
	    break;
	}

}

int
main( int argc, char **argv )
{

    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
#endif
    glutInitWindowSize( 500, 500 );

    mainWindow = glutCreateWindow( "Assignment 3" );

#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif

	    glutInitWindowSize( w, h );

	    initMainWindow();
	    glutDisplayFunc( displayMainWindow );
	    glutKeyboardFunc( keyboard );



	    subWindow1 = glutCreateSubWindow(mainWindow,
	     0,0,200, 100);
	    glutSetWindow(subWindow1);
	    initSubWindow();
	     // Must register a display func for each window
	     glutDisplayFunc(displaySubWindow);

		glutIdleFunc(idle);

		// Create a menu on the sub window only that allows the user to change the subwindow's
		// background color.

		glutCreateMenu(menu_chooser);
		glutAddMenuEntry("Green",1);
		glutAddMenuEntry("Light Blue",2);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

		// Draw a circle and triangle, which have the same color, in a separate window entitled “window 2”. Allow the user to change the color of these objects by typing a key, 'r' - red, 'g' - green, 'b' - blue, 'y' - yellow, 'o' - orange, 'p' - purple, 'w' - white.
		window2 = glutCreateWindow( "Window 2" );
	    glutInitWindowSize( w, h );

	    initWindow2();

	    glutDisplayFunc( displayWindow2 );
	    glutKeyboardFunc( keyboardWindow2 );
		glutIdleFunc(idle2);


		std::cout << "Right click on subwindow to select from menu allowing color change" << std::endl;
		std::cout << "Change window 2 object colors by typing objects by typing a key, 'r' - red, 'g' - green, 'b' - blue, 'y' - yellow, 'o' - orange, 'p' - purple, 'w' - white." << std::endl;

	    glutMainLoop();
	    return 0;
}
