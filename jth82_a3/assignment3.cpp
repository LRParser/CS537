// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <math.h>
#include <time.h>


const int ellipsePoints = 100;
const int pointsPerSquare = 12;
const int numSquares = 6;
const int squarePoints = pointsPerSquare * numSquares;

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
float window2Red = 1.0f;
float window2Green= 0.0f;
float window2Blue = 0.0f;

float window2TriangleX[3];
float window2TriangleY[3];

struct timespec ts_start;
struct timespec ts_end;

bool animationEnabled = false;


window_info subWindowInfo;

//--------------------------------------------------------------------------

vec3 vertices[squarePoints];
vec3 colors[squarePoints];

vec3 subVertices[ellipsePoints];
vec3 subColors[ellipsePoints];
vec3 squareColor = vec3(1.0,1.0,1.0);

const int windowTwoPoints = 103;
vec3 windowTwoVertices[windowTwoPoints];
vec3 windowTwoColors[windowTwoPoints];

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
			colors[i] = vec3(window2Red,window2Green,window2Blue);
		}
		i++;
		angle += 0.0628;
	}
}

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
	createCircle(windowTwoVertices,windowTwoColors,.3,0,100,1.2,false,.5,.5);

	// Triangle
    window2TriangleX[0] = 0;
	window2TriangleX[1] = .75;
	window2TriangleX[2] = 0;
    window2TriangleY[0] = 0;
	window2TriangleY[1] = 0;
	window2TriangleY[2] = .75;

    windowTwoVertices[100] =  vec3(window2TriangleX[0],window2TriangleY[0],0);
    windowTwoVertices[101] = vec3(window2TriangleX[1],window2TriangleY[1],0);
    windowTwoVertices[102] = vec3(window2TriangleX[2],window2TriangleY[2],0);

	for(int i = 0; i < 103; i++) {
		windowTwoColors[i] = vec3(window2Red,window2Green,window2Blue);
	}


    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );


    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    size_t totalSize = sizeof(windowTwoVertices) +
    		sizeof(windowTwoColors);

    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_DYNAMIC_DRAW );

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
                           BUFFER_OFFSET(sizeof(windowTwoVertices)) );

    glEnable( GL_DEPTH_TEST );


    glClearColor( 1.0, 1.0, .5, 0.0 ); // green background
}

void displayWindow2() {

    size_t totalSize = sizeof(windowTwoVertices) +
    		sizeof(windowTwoColors);
    // Rotate counter-clockwise


    glBufferData( GL_ARRAY_BUFFER, totalSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(windowTwoVertices),
    		windowTwoVertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(windowTwoVertices), sizeof(windowTwoColors),
    		windowTwoColors );



    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glClearColor( 1.0, 1.0, .5, 0.0 ); // green background

    glDrawArrays( GL_TRIANGLE_FAN, 0, 100 );    // draw the shaded circle
    glFlush();
    glDrawArrays( GL_TRIANGLES, 100, 3 );    // draw the shaded circle

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
			window2Red = 1;
			window2Green = 0;
			window2Blue = 0;
			break;
		case 'g':
			pressed = true;
			window2Red = 0;
			window2Green = 1;
			window2Blue = 0;
			break;
		case 'b':
			pressed = true;
			window2Red = 0;
			window2Green = 0;
			window2Blue = 1;
			break;
		case 'y':
			pressed = true;
			window2Red = 1;
			window2Green = 1;
			window2Blue = 0;
			break;
		case 'o':
			pressed = true;
			window2Red = 1;
			window2Green = 0.5;
			window2Blue = 0;
			break;
		case 'p':
			pressed = true;
			window2Red = 1.0f;
			window2Green = (1.0 * (192.0/256.0));
			window2Blue = (1.0f * (203.0/256.0));
			break;
		case 'w':
			pressed = true;
			window2Red = 1;
			window2Green = 1;
			window2Blue = 1;
			break;
    }
    if(pressed) {

    	for(int i = 0; i < 103; i++) {
    		windowTwoColors[i] = vec3(window2Red,window2Green,window2Blue);
    	}
        glutSetWindow(window2);
        glutPostRedisplay();
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

		    // Trig ref online = http://petercollingridge.appspot.com/3D-tutorial/rotating-objects
		    float sin_t = sin(Theta[Axis]);
		    float cos_t = cos(Theta[Axis]);


			for(int i = 0; i < squarePoints;i++ ) {

		        float x = vertices[i].x;
		        float y = vertices[i].y;
		        vertices[i].x = x * cos_t - y * sin_t;
		        vertices[i].y = y * cos_t + x * sin_t;
			}

			// Clockwise

		    sin_t = sin(-1 * Theta[Axis]);
		    cos_t = cos(-1 * Theta[Axis]);

			for(int i = 0; i < 3; i++) {
		        float x = window2TriangleX[i];
		        float y = window2TriangleY[i];
		        window2TriangleX[i] = x * cos_t - y * sin_t;
		        window2TriangleY[i] = y * cos_t + x * sin_t;
			}

		    windowTwoVertices[100] =  vec3(window2TriangleX[0],window2TriangleY[0],0);
		    windowTwoVertices[101] = vec3(window2TriangleX[1],window2TriangleY[1],0);
		    windowTwoVertices[102] = vec3(window2TriangleX[2],window2TriangleY[2],0);

		    // Pulsing circle
			createCircle(windowTwoVertices,windowTwoColors,.3   * sin(Theta[Axis]),0,100,1.2,false,.5,.5);

			glutSetWindow(window2);
		    glutPostRedisplay();
		    glutSetWindow(mainWindow);
		    glutPostRedisplay();

		    clock_gettime(CLOCK_MONOTONIC, &ts_end);
		}
	}
}


//----------------------------------------------------------------------------

void menu_chooser_subwindow(int id) {
	bool choiceSet = false;
	switch(id)
	{
	case 1:
		blueValue = 0.0f;
	    glClearColor( 0.0,1.0,blueValue,0.0f ); // green background
	    choiceSet = true;
	    break;

	case 2:
		blueValue = 1.0f;
	    glClearColor( 0.0,1.0,blueValue,0.0f ); // green background
	    choiceSet = true;
	    break;
	}
	if(choiceSet) {
		glutPostRedisplay();
	}
}

void setSquaresColor(vec3 color) {
	// Squares 0, 2, 4 need coloring
	for(int i = 0; i < 12; i++) {
		colors[i] = vec3(color);
	}
	for(int i = 24; i < 36; i++) {
		colors[i] = vec3(color);
	}
	for(int i = 48; i < 60; i++) {
		colors[i] = vec3(color);
	}

}

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
		squareColor = vec3(1.0,1.0,1.0);
		setSquaresColor(squareColor);
	    choiceSet = true;
	    break;
	case 4:
		squareColor = vec3(1.0,0.0,0.0);
		setSquaresColor(squareColor);
	    choiceSet = true;
	    break;
	case 5:
		squareColor = vec3(0.0,1.0,0.0);
		setSquaresColor(squareColor);

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
		glutIdleFunc(idle);

		int animationMenu = glutCreateMenu(menu_chooser_mainwindow);
		glutAddMenuEntry("Start Animation",1);
		glutAddMenuEntry("Stop Animation",2);

		int squareColorMenu = glutCreateMenu(menu_chooser_mainwindow);
		glutAddMenuEntry("White",3);
		glutAddMenuEntry("Red",4);
		glutAddMenuEntry("Green",5);

		int mainMenu = glutCreateMenu(menu_chooser_mainwindow);
		glutAddSubMenu("Animation", animationMenu);
		glutAddSubMenu("Square Color", squareColorMenu);
		glutAttachMenu(GLUT_RIGHT_BUTTON);



	    subWindow1 = glutCreateSubWindow(mainWindow,
	     0,0,200, 100);
	    glutSetWindow(subWindow1);
	    initSubWindow();
	     // Must register a display func for each window
	     glutDisplayFunc(displaySubWindow);




		// Create a menu on the sub window only that allows the user to change the subwindow's
		// background color.

		glutCreateMenu(menu_chooser_subwindow);
		glutAddMenuEntry("Green",1);
		glutAddMenuEntry("Light Blue",2);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

		// Draw a circle and triangle, which have the same color, in a separate window entitled “window 2”. Allow the user to change the color of these objects by typing a key, 'r' - red, 'g' - green, 'b' - blue, 'y' - yellow, 'o' - orange, 'p' - purple, 'w' - white.

		window2 = glutCreateWindow( "Window 2" );
	    glutInitWindowSize( w, h );

	    initWindow2();

	    glutDisplayFunc( displayWindow2 );
	    glutKeyboardFunc( keyboardWindow2 );

		std::cout << "Usage: Click main window named Assignment 3. Right click and choose to either start/stop animation or change square colors" << std::endl;
		std::cout << "Right click on subwindow (top-left of main window) to select from menu allowing color change of ellipse" << std::endl;
		std::cout << "Change window 2 object colors by typing objects by typing a key, 'r' - red, 'g' - green, 'b' - blue, 'y' - yellow, 'o' - orange, 'p' - purple, 'w' - white." << std::endl;

	    glutMainLoop();
	    return 0;
}
