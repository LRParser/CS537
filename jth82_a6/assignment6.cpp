// Make a 3D cube

#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>

struct timespec ts_start;
struct timespec ts_end;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

struct faceInfo {
	float face1;
	float face2;
	float face3;
};

mat4 TransformMatrix;
GLuint transformMatrix;


// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
bool isPerspective = true;

// For Ortho coordinates
float left = -1.0f;
float right = 1.0f;
float bottom = -1.0f;
float top = 1.0f;
float near = 0.0f;
float far = 10.0f;

// Copied from Lecture 8 slides as described in assignment

const int NumVertices = 10000; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int NumVerticesUsed = 24;

vec4 smfVertices[NumVertices];
struct faceInfo smfFaces[NumVertices];

vec4 points[10000];
vec4 colors[10000];

vec3 ScaleFactors = vec3(1.0f, 1.0f, 1.0f);
vec3  RotationFactors = vec3(0.0f,0.0f,0.0f);
vec3  TranslationFactors = vec3(0.0f,0.0f,0.0f);

vec3 EyeVector = vec3(1.0f,1.0f,3.0f);

float Rho = 4.0; // Radius in degrees
float Phi = 90; // Zenith angle in degrees
float Theta = 1; // Longitude angle in degrees
float Height = 0;

float RhoDelta = 1;
int Delta = 5;
float PhiDelta = 5;
float HeightDelta = .1;
float ParallelDelta = 2;

int mainWindow;

int w = 500;
int h = 500;
int border = 50;

GLint windowHeight, windowWidth;

float radians(float degrees) {
	return (M_PI * degrees) / 180;
}

vec3 radians(vec3 degrees) {
        return (M_PI * degrees) / 180;
}

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

    transformMatrix = glGetUniformLocation(program, "transformMatrix");


    glEnable( GL_DEPTH_TEST );

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

void calculateEyeVector() {

	// http://gamedev.stackexchange.com/questions/5766/camera-rotation-using-angles
	float X, Y, Z;

	X = Rho * sin(radians(Phi)) * cos(radians(Theta));
	Y = Rho * cos(radians(Phi)) + Height;
	Z = Rho * sin(radians(Phi)) * sin(radians(Theta));

	EyeVector.x = X;
	EyeVector.y = Y;
	EyeVector.z = Z;


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

   mat4 Projection;
   if(isPerspective) {
	   Projection = Perspective(45.0f, 4/3, 0.1f, 100.0f);
   }
   else {
	   Projection = Ortho(left,right,bottom,top,near,far); // In world coordinates

   }


   calculateEyeVector();

   // Camera matrix
   mat4 View = LookAt(
	EyeVector,
    vec3(0,0,0),
    vec3(0,1,0)
       );

   // Move model to the origin
   mat4 Model = mat4(1.0f);

   TransformMatrix = Projection * View * Model;

   glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );

    glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );
    glFlush();

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

	// We either manipulate ScaleFactor, RotationFactor, or TranslateFactor, depending on current operation

    switch ( key ) {


    case '1' :
    	// Increase Height
    	pressed = true;
    	Height += HeightDelta;
    	break;
    case '2' :
    	// Decrease height
    	pressed = true;
    	Height -= HeightDelta;
    	break;
    case '3' :
    	// Increase orbit radius / distance of camera
    	pressed = true;

    	if(isPerspective) {
        	Rho += RhoDelta;
        	if(Rho >= 360) {
        		Rho = 360;
        	}
    	}
    	else {
    		near += ParallelDelta;
    		far += ParallelDelta;
    	}

    	break;
    case '4' :
    	pressed = true;
    	if(isPerspective) {
        	Rho -= RhoDelta;
        	if(Rho <= 1) {
        		Rho = 1;
        	}
    	}
    	else {
    		near -= ParallelDelta;
    		far -= ParallelDelta;

    	}


    	break;
    case '5' :
    	// Increase phi
    	pressed = true;
    	Phi += PhiDelta;
    	if(Phi >= 360) {
    		Phi = 360;
    	}
    	break;
    case '6' :
    	// Decrease phi
    	pressed = true;
    	Phi -= PhiDelta;
    	if(Phi <= 0) {
    		Phi = 0;
    	}
    	break;
    case '7' :
    	// Set perspective projection
    	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    	isPerspective = true;

    	pressed = true;
    	break;
    case '8' :
    	// Decrease phi
    	isPerspective = false;
    	pressed = true;
    	break;
    case '9' :
    	// Rotate counterclockwise
    	Theta += 10;
    	if(Theta == 360) {
    		Theta = 0;
    	}
    	pressed = true;

    	break;
    case '0' :
    	Theta -= 10;
    	if(Theta < 0) {
    		Theta = 0;
    	}
    	pressed = true;

    	break;

	case 'q':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    }
    if(pressed) {

    	calculateEyeVector();

        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }


}


void idle() {


}


//----------------------------------------------------------------------------

void readSMF(char* fileName) {


	// Read in the SMF file
			std::ifstream infile(fileName);

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
			}



			int totalPoints = 0;
			for(int i = 0; i < numSmfFaces; i++) {
				faceInfo currentFace = smfFaces[i];
				// Find the vertices it specifies and add them to vertices
				// Subtract 1 because SMF is 1-indexed
				// printf("Building face: %d - %f %f %f\n",i,currentFace.face1,currentFace.face2,currentFace.face3);
				int index1 = currentFace.face1;
				vec4 vertex1 = smfVertices[index1 - 1];

				int index2 = currentFace.face2;
				vec4 vertex2 = smfVertices[index2 - 1];

				int index3 = currentFace.face3;
				vec4 vertex3 = smfVertices[index3 - 1];

				int currentOffset = i * 3;

				points[currentOffset] = vertex1;
				points[currentOffset + 1] = vertex2;
				points[currentOffset + 2] = vertex3;

				// Calculate the surface normal, ref alg from Kronos Group
				/*
				 * So for a triangle p1, p2, p3, if the vector U = p2 - p1 and the vector V = p3 - p1 then the normal N = U X V and can be calculated by:
					Nx = UyVz - UzVy
					Ny = UzVx - UxVz
					Nz = UxVy - UyVx
				 */

				vec4 U = vertex2 - vertex1;
				vec4 V = vertex3 - vertex1;

				float normalX = (U.y*V.z) - (U.z*V.y);
				float normalY = (U.z*V.x) - (U.x*V.z);
				float normalZ = (U.x*V.y) - (U.y*V.x);

				float normalDenominator = sqrt((normalX*normalX) + (normalY*normalY) + (normalZ*normalZ));
				normalX = normalX / normalDenominator;
				normalY = normalY / normalDenominator;
				normalZ = normalZ  / normalDenominator;

				//printf("Color is: %f, %f, %f\n",normalX,normalY,normalZ);

				vec4 normalNormalized = vec4(normalX,normalY,normalZ,1.0);

				colors[currentOffset] = normalNormalized;
				colors[currentOffset + 1] = normalNormalized;
				colors[currentOffset + 2] = normalNormalized;
				totalPoints += 3;
			}

			for(int i = 0; i < totalPoints; i++) {
				//printf("Point created: %f, %f, %f\n",points[i].x,points[i].y,points[i].z);
			}

			//printf("Total points created: %d\n",totalPoints);
			NumVerticesUsed = totalPoints;


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

    mainWindow = glutCreateWindow( "Assignment 5" );

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
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);


		if(argc == 1) {
			std::cout << "You must specify an SMF file" << std::endl;
		}
		else {
			char* fileName = argv[1];
			std::cout << "Filename is: " << argv[1] << std::endl;
			readSMF(fileName);
			std::cout << "Press: 1 - To increase camera height" << std::endl;
			std::cout << "Press: 2 - To decrease camera height" << std::endl;
			std::cout << "Press: 3 - To increase orbit radius" << std::endl;
			std::cout << "Press: 4 - To decrease orbit radius" << std::endl;
			std::cout << "Press: 5 - To increase cylinder angle" << std::endl;
			std::cout << "Press: 6 - To decrease cylinder angle" << std::endl;
			std::cout << "Press: 7 - To switch to perspective projection mode (default)" << std::endl;
			std::cout << "Press: 8 - To switch to parallel projection mode" << std::endl;
			std::cout << "Press: 9 - To rotate camera counterclockwise" << std::endl;
			std::cout << "Press: 0 - To rotate camera clockwise" << std::endl;
			std::cout << "Press: q - To exit the program" << std::endl;


		}



	    glutMainLoop();
	    return 0;
}
