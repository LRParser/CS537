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




enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
float delta = .1f;


GLuint scaleFactors; // Location of "ScaleFactors" uniform variable

vec3 ScaleFactors = vec3(1.0f, 1.0f, 1.0f);
vec3  RotationFactors = vec3(0.0f,0.0f,0.0f);
vec3  TranslationFactors = vec3(0.0f,0.0f,0.0f);




mat4 TransformMatrix;
GLuint transformMatrix;

vec3 *currentOperation = &ScaleFactors;



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
vec3 radians(vec3 degrees) {
	return (M_PI * degrees) / 180;
}

vec3 cos(vec3 angles) {
	angles.x = cos(angles.x);
	angles.y = cos(angles.y);
	angles.z = cos(angles.z);

	return angles;
}

vec3 sin(vec3 angles) {
	angles.x = sin(angles.x);
	angles.y = sin(angles.y);
	angles.z = sin(angles.z);

	return angles;
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

    transformMatrix = glGetUniformLocation(program, "transformMatrix");



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

        printf("X scale set to: %f\n",ScaleFactors.x);
        mat4 scaleMatrix;
        scaleMatrix[0][0] = ScaleFactors.x;
        scaleMatrix[1][1] = ScaleFactors.y;
        scaleMatrix[2][2] = ScaleFactors.z;

        // Rotate
        vec3 angles = radians(-1.0f * RotationFactors);

        vec3 c = cos( angles );
        vec3 s = sin( angles );

        mat4 rx = mat4( 1.0,  0.0,  0.0, 0.0,
    		    0.0,  c.x,  s.x, 0.0,
    		    0.0, -s.x,  c.x, 0.0,
    		    0.0,  0.0,  0.0, 1.0 );
        mat4 ry = mat4( c.y, 0.0, -s.y, 0.0,
    		    0.0, 1.0,  0.0, 0.0,
    		    s.y, 0.0,  c.y, 0.0,
    		    0.0, 0.0,  0.0, 1.0 );

        mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
    		    s.z,  c.z, 0.0, 0.0,
    		    0.0,  0.0, 1.0, 0.0,
    		    0.0,  0.0, 0.0, 1.0 );

        mat4 rotationMatrix = rx * ry * rz;

        // TTranslate
        mat4 translationMatrix;
        translationMatrix[0][3] = TranslationFactors.x;
        translationMatrix[1][3] = TranslationFactors.y;
        translationMatrix[2][3] = TranslationFactors.z;

        mat4 newMatrix = scaleMatrix * rotationMatrix * translationMatrix;
        TransformMatrix = newMatrix;

        glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );
        printf("Calling Draw and SwapBuffers");

    glDrawArrays( GL_TRIANGLES, 0, squarePoints );

    glutSwapBuffers();

}


void reshape(GLsizei w, GLsizei h) {
	windowWidth = w;
	windowHeight = h;
	glViewport(0,0,windowWidth,windowHeight);
}

void resetAllTransformations() {
	// Reset to original points
	ScaleFactors.x = 1.0f;
	ScaleFactors.y = 1.0f;
	ScaleFactors.z = 1.0f;
	RotationFactors.x = 0.0f;
	RotationFactors.y = 0.0f;
	RotationFactors.z = 0.0f;
	TranslationFactors.x = 0.0f;
	TranslationFactors.y = 0.0f;
	TranslationFactors.z = 0.0f;
}

void
keyboard( unsigned char key, int x, int y )
{
	// Define six keys for increasing and decreasing the X,Y,Z components of the current transformation.
	// The cube should only be transformed with each key stroke.
	bool pressed = false;

	// We either manipulate ScaleFactor, RotationFactor, or TranslateFactor, depending on current operation

    switch ( key ) {
		case 'q':
			// Exit
			exit( EXIT_SUCCESS );
			break;
		case '1':
	    	// Decrease x component of current operation
			currentOperation->x -= delta;
			pressed = true;
			break;
		case '2':
			// Increase x component of transform
			currentOperation->x += delta;
			printf("Increased Xaxis component\n");
			pressed = true;
			break;

		case '3':
			currentOperation->y -= delta;
			pressed = true;
			break;
		case '4':
			currentOperation->y += delta;
			pressed = true;
			break;
		case '5':
			currentOperation->z -= delta;
			pressed = true;
			break;
		case '6':
			currentOperation->z += delta;
			pressed = true;
			break;
		case '7':
			delta -= .1;
			if(delta <= 0) {
				delta = .01; // Don't allow delta of less than minimum increment
			}
			printf("Delta is: %f\n",delta);

			pressed = true;
			break;
		case '8':
			delta += .1;
			printf("Delta is: %f\n",delta);
			pressed = true;
			break;
		case 'r':
			resetAllTransformations();
			pressed = true;
			break;

    }
    if(pressed) {

        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }


}


void idle() {


}


//----------------------------------------------------------------------------


void menu_chooser_mainwindow(int id) {
	bool choiceSet = false;
	switch(id)
	{
	case 1:
		currentOperation = &ScaleFactors;
	    choiceSet = true;
	    break;

	case 2:
		currentOperation = &RotationFactors;
	    choiceSet = true;
	    break;
	case 3:
		currentOperation = &TranslationFactors;
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

		glutCreateMenu(menu_chooser_mainwindow);
		glutAddMenuEntry("Set Scale Transform",1);
		glutAddMenuEntry("Set Rotate Transform",2);
		glutAddMenuEntry("Set Translate Transform",3);
		glutAttachMenu(GLUT_RIGHT_BUTTON);



		std::cout << "Usage: Right Click main window named Assignment 4. Right click and choose to select current transformation to be applied (scale, rotate or translate)" << std::endl;
		std::cout << "You can decrease or increase the transform amount in X, Y and Z axis by pressing either 1 or 2 (decease or increase in X), 3 or 4 (decrease or increase in Y) or 5 and 6 (decrease or increase in Z) number keys, respectively" << std::endl;
		std::cout << "You can decrease or increase the delta applied to the current transform by pressing either 7 or 8, respectively" << std::endl;
		std::cout << "Press r to reset all transformations" << std::endl;

	    glutMainLoop();
	    return 0;
}
