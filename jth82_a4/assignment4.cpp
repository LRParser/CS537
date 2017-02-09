// Make a 3D cube

#include "Angel.h"
#include <math.h>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;


// Copied from Lecture 8 slides as described in assignment

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

int mainWindow;

int w = 500;
int h = 500;
int border = 50;

GLint windowHeight, windowWidth;




enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
int opIndex = 0;

GLuint scaleFactors; // Location of "ScaleFactors" uniform variable

vec3 ScaleFactors = vec3(1.0f, 1.0f, 1.0f);
vec3  RotationFactors = vec3(0.0f,0.0f,0.0f);
vec3  TranslationFactors = vec3(0.0f,0.0f,0.0f);


mat4 TransformMatrix;
GLuint transformMatrix;

vec3 *currentOperation = &ScaleFactors;
float deltas[3] = { .1f,1.0f,.05f};



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
	colorcube();
    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );


    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(colors), colors );


    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    //  glUseProgram( program );  // This is called in InitShader

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(points)) );

    transformMatrix = glGetUniformLocation(program, "transformMatrix");



    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background


}

vec3 radians(vec3 degrees) {
	return (M_PI * degrees) / 180;
}



void
displayMainWindow( void )
{

    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(colors), colors );

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

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

        // Translate to origin
        mat4 translationMatrixOrigin;
        translationMatrixOrigin[0][3] = 0.0f;
        translationMatrixOrigin[1][3] = 0.0f;
        translationMatrixOrigin[2][3] = 0.0f;

        mat4 newMatrix = translationMatrixOrigin * scaleMatrix * rotationMatrix * translationMatrix;
        TransformMatrix = newMatrix;

        glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

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
	deltas[0] = .1f;
	deltas[1] = 1.0f;
	deltas[2] = .05f;
	opIndex = 0;

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
			currentOperation->x -= deltas[opIndex];
			pressed = true;
			break;
		case '2':
			// Increase x component of transform
			currentOperation->x += deltas[opIndex];
			pressed = true;
			break;

		case '3':
			currentOperation->y -= deltas[opIndex];
			pressed = true;
			break;
		case '4':
			currentOperation->y += deltas[opIndex];
			pressed = true;
			break;
		case '5':
			currentOperation->z -= deltas[opIndex];
			pressed = true;
			break;
		case '6':
			currentOperation->z += deltas[opIndex];
			pressed = true;
			break;
		case '7':

			deltas[opIndex] -= .1;

			pressed = true;
			break;
		case '8':

			deltas[opIndex] += .1;
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
		opIndex = 0;
	    choiceSet = true;
	    break;

	case 2:
		currentOperation = &RotationFactors;
		opIndex = 1;
	    choiceSet = true;
	    break;
	case 3:
		currentOperation = &TranslationFactors;
		opIndex = 2;
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

		std::cout << "Usage: Right Click main window named Assignment 4 and choose to select current transformation to be applied (scale, rotate or translate)" << std::endl;
		std::cout << "Press: 1 - to increase transform in X axis" << std::endl;
		std::cout << "Press: 2 - to decrease transform in X axis" << std::endl;
		std::cout << "Press: 3 - to increase transform in Y axis" << std::endl;
		std::cout << "Press: 4 - to decrease transform in Y axis" << std::endl;
		std::cout << "Press: 5 - to increase transform in Z axis" << std::endl;
		std::cout << "Press: 6 - to decrease transform in Z axis" << std::endl;
		std::cout << "Press: 7 - to increase delta for current transform" << std::endl;
		std::cout << "Press: 8 - to decrease delta for current transform" << std::endl;
		std::cout << "Press: r - to reset all transformations and deltas to default" << std::endl;
		std::cout << "Press: q - to quit" << std::endl;

	    glutMainLoop();
	    return 0;
}
