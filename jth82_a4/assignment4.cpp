// Make a 3D cube

#include "Angel.h"
#include <math.h>
#include <time.h>


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

struct timespec ts_start;
struct timespec ts_end;

bool animationEnabled = false;
const int pointsPerSquare = 12;
const int numSquares = 6;
const int squarePoints = pointsPerSquare * numSquares;
vec3 squareColor = vec3(1.0,1.0,1.0);

const int squaresStartIdx = 0;
const int squaresEndIdx = squarePoints -1;

//--------------------------------------------------------------------------

vec3 vertices[squarePoints];
vec3 colors[squarePoints];


int mainWindow;

int w = 500;
int h = 500;
int border = 50;

GLint windowHeight, windowWidth;


// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Zaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };
GLuint  theta;  // The location of the "theta" shader uniform variable


void createSquare(int startVertex, float scaleFactor, float centroidX, float centroidY, float xScale, float yScale, bool isWhite, float zIndex) {


	vec3 blackColor = vec3(0.0,0.0,0.0);
	vec3 color = isWhite ? squareColor : blackColor;

	int currentVertex = startVertex;

	// Make square from 4 triangles all "pointing" towards center point

	// Triangle 1
	vertices[currentVertex] = vec3(centroidX,centroidY,zIndex);
	colors[currentVertex] = vec3(color);
	currentVertex++;

	float normalizedXScale = scaleFactor * xScale;
	float normalizedYScale = scaleFactor * yScale;

	vertices[currentVertex] = vec3(centroidX - normalizedXScale, centroidY - normalizedYScale,zIndex);
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

void drawSquares() {
	float overallScale = .5;

	createSquare(squaresStartIdx,overallScale,0,0,.7,.7,true,0.0);
	createSquare(squaresStartIdx + 1 * pointsPerSquare,overallScale,0,0,.6,.6,false,0.1);
	createSquare(squaresStartIdx + 2 * pointsPerSquare,overallScale,0,0,.5,.5,true,0.2);
	createSquare(squaresStartIdx + 3 * pointsPerSquare,overallScale,0,0,.4,.4,false,0.3);
	createSquare(squaresStartIdx + 4 * pointsPerSquare,overallScale,0,0,.3,.3,true,0.4);
	createSquare(squaresStartIdx + 5 * pointsPerSquare,overallScale,0,0,.2,.2,false,0.5);

}

void
initMainWindow( void )
{

	// We need to now draw 6 squares to make a full 3D cube
	drawSquares();
    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );


    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );




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
    size_t totalSize = sizeof(vertices) +
    		sizeof(colors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertices),
    		vertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors),
    		colors );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glUniform3fv( theta, 1, Theta );
    glDrawArrays( GL_TRIANGLES, 0, squarePoints );

    glutSwapBuffers();

}


void reshape(GLsizei w, GLsizei h) {
	windowWidth = w;
	windowHeight = h;
	glViewport(0,0,windowWidth,windowHeight);
}

void
keyboard( unsigned char key, int x, int y )
{
	// Define six keys for increasing and decreasing the X,Y,Z components of the current transformation.
	// The cube should only be transformed with each key stroke.
	bool pressed = false;
    switch ( key ) {
		case 'q':
			// Exit
			exit( EXIT_SUCCESS );
			break;
		case '1':
	    	// Increase x component of transform
			exit( EXIT_SUCCESS );
			break;
		case '2':
			// Decrease x component of transform
			pressed = true;
			break;
		case '3':

			pressed = true;
			break;
		case '4':
			pressed = true;
			break;
		case '5':
			pressed = true;
			break;
		case '6':
			pressed = true;
			break;
		case '+':
			pressed = true;
			break;
		case '-':
			pressed = true;
			break;
    }
    if(pressed) {

        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }


}


void idle() {
	// Measure time and then update based on this

	if(animationEnabled) {

	    clock_gettime(CLOCK_MONOTONIC, &ts_start);

		long int elapsedTime = (ts_end.tv_sec - ts_start.tv_sec);

		if(elapsedTime > 1 || elapsedTime < 0) {

		Theta[Axis] += 0.05;

		    if ( Theta[Axis] > 360.0 ) {
			Theta[Axis] -= 360.0;
		    }


		    glutSetWindow(mainWindow);
		    glutPostRedisplay();

		    clock_gettime(CLOCK_MONOTONIC, &ts_end);
		}
	}
}


//----------------------------------------------------------------------------


void menu_chooser_mainwindow(int id) {
	bool choiceSet = false;
	switch(id)
	{
	case 1:
		animationEnabled = true;
	    choiceSet = true;
	    break;

	case 2:
		animationEnabled = false;
	    choiceSet = true;
	    break;
	case 3:

	    choiceSet = true;
	    break;
	case 4:

	    choiceSet = true;
	    break;
	case 5:

	    choiceSet = true;
	    break;
	}
	if(choiceSet) {
		glutPostRedisplay();
	}
}


int
main( int argc, char **argv )
{

    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
#endif
    glutInitWindowSize( 500, 500 );

    mainWindow = glutCreateWindow( "Assignment 4" );

#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif

	    glutInitWindowSize( w, h );

	    initMainWindow();
	    glutDisplayFunc( displayMainWindow );
	    glutKeyboardFunc( keyboard );
		glutIdleFunc(idle);

		int animationMenu = glutCreateMenu(menu_chooser_mainwindow);
		glutAddMenuEntry("Apply Scale Transform",1);
		glutAddMenuEntry("Apply Rotate Transform",2);
		glutAddMenuEntry("Apply Translate Transform",3);
		glutAttachMenu(GLUT_RIGHT_BUTTON);



		std::cout << "Usage: Right Click main window named Assignment 4. Right click and choose to select current transformation to be applied (scale, rotate or translate)" << std::endl;
		std::cout << "You can (increase,decrease) the transform amount in X, Y and Z axis by pressing (1,2), (3,4) or (5,6) number keys, respectively" << std::endl;
		std::cout << "You can increase or decrease the delta applied to the current transform by pressing either '+' or '-'" << std::endl;

	    glutMainLoop();
	    return 0;
}
