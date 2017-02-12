// Make a 3D cube

#include "Angel.h"
#include <math.h>
#include <fstream>


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

struct faceInfo {
	float face1;
	float face2;
	float face3;
};


// Copied from Lecture 8 slides as described in assignment

const int NumVertices = 1000; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int NumVerticesUsed = 24;

vec4 smfVertices[NumVertices];
struct faceInfo smfFaces[NumVertices];

//vec4 *points;
// vec4 *colors;

vec4 points[1000];
vec4 colors[1000];

/*
vec4 points[6] = {
    vec4(-.75,-.75,1,1), vec4(-.75,0,1,1),vec4(0,-.75,1,1),
    vec4(-.75,0,1,1 ), vec4(0,-.75,1,1), vec4( 0, 0,1,1 )
};

vec4 colors[6] = {
    vec4(1,0,0,0), vec4(1,0,0,0),vec4(1,0,0,0),
	vec4(1,0,0,0), vec4(1,0,0,0), vec4(1,0,0,0)
};
*/

int mainWindow;

int w = 500;
int h = 500;
int border = 50;

GLint windowHeight, windowWidth;


void
initMainWindow( void )
{

	// We need to now draw 6 squares to make a full 3D cube


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




    // glEnable( GL_DEPTH_TEST );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // black background

    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(colors), colors );

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );
    glFlush();

    // glutSwapBuffers();

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

    glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );
    glFlush();

    // glutSwapBuffers();

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

	// We either manipulate ScaleFactor, RotationFactor, or TranslateFactor, depending on current operation

    switch ( key ) {
		case 'q':
			// Exit
			exit( EXIT_SUCCESS );
			break;

    }
    if(pressed) {

        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }


}


void idle() {
	glutSwapBuffers();

}


//----------------------------------------------------------------------------

void readSMF() {


	// Read in the SMF file
			std::ifstream infile("bound-bunny_200.smf");

			char a;
			float b, c, d;
			int numSmfVertices = 0;
			int numSmfFaces = 0;
			while (infile >> a >> b >> c >> d)
			{
				if(a == 'v') {
					smfVertices[numSmfVertices] = vec4(b,c,d,1.0);
					numSmfVertices++;
				}
				else if(a == 'f') {

					faceInfo f = {int(b),int(c),int(d)};
					smfFaces[numSmfFaces] = f;
					numSmfFaces++;
				}
			    printf("%c, %f, %f, %f\n",a, b,c,d);
			}

			printf("NumVerticesUsed is: %d\n",NumVerticesUsed);

			// points = (vec4*) malloc(sizeof(vec4) * NumVerticesUsed);
			// colors = (vec4*) malloc(sizeof(vec4) * NumVerticesUsed);

			// Now, take the faces to order the vertices

			int totalPoints = 0;
			for(int i = 0; i < numSmfFaces; i++) {
				faceInfo currentFace = smfFaces[i];
				// Find the vertices it specifies and add them to vertices
				// Subtract 1 because SMF is 1-indexed
				printf("Building face: %d - %f %f %f\n",i,currentFace.face1,currentFace.face2,currentFace.face3);
				int index1 = currentFace.face1;
				vec4 vertex1 = smfVertices[index1 - 1];

				int index2 = currentFace.face2;
				vec4 vertex2 = smfVertices[index2 - 1];

				int index3 = currentFace.face3;
				vec4 vertex3 = smfVertices[index3 - 1];

				printf("Looking at indices: %d, %d, %d\n",index1,index2,index3);

				int currentOffset = i * 3;

				points[currentOffset] = vertex1;
				points[currentOffset + 1] = vertex2;
				points[currentOffset + 2] = vertex3;

				colors[currentOffset] = vec4(1.0,0.0,0.0,1.0);
				colors[currentOffset + 1] = vec4(1.0,0.0,0.0,1.0);
				colors[currentOffset + 2] = vec4(1.0,0.0,0.0,1.0);
				totalPoints += 3;
			}

			for(int i = 0; i < totalPoints; i++) {
				printf("Point created: %f, %f, %f\n",points[i].x,points[i].y,points[i].z);
			}

			printf("Total points created: %d\n",totalPoints);
			NumVerticesUsed = totalPoints;


}

int
main( int argc, char **argv )
{

	readSMF();

    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA );
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




		std::cout << "Usage Info: TBD" << std::endl;


	    glutMainLoop();
	    return 0;
}
