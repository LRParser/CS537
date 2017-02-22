// Make a 3D cube

#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

struct timespec ts_start;
struct timespec ts_end;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

class face {
public:
	int faceIdx;
	int firstVertexIndex;
	int secondVertexIndex;
	int thirdVertexIndex;
	vec4 firstVertex;
	vec4 secondVertex;
	vec4 thirdVertex;
	vec4 color;
};


std::map<vec4,std::vector<face> > vectorFaceMapping;

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
std::vector<face> smfFaces;

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

vec3 light_i_ambient;
vec3 light_i_diffuse;
vec3 light_i_specular;

float colorScaleFactor = 20;

GLint windowHeight, windowWidth;

float radians(float degrees) {
	return (M_PI * degrees) / 180;
}

vec3 radians(vec3 degrees) {
        return (M_PI * degrees) / 180;
}

vec4 vAbs(vec4 input) {
	vec4 absVec = vec4(std::abs(input.x),std::abs(input.y),
			std::abs(input.z),1.0);
	return absVec;
}

vec4 vScale(vec4 input, float scaleFactor) {
	vec4 scaleVec = vec4(scaleFactor * input.x,scaleFactor * input.y,scaleFactor * input.z,1.0);
	return scaleVec;
}


void
initMainWindow( void )
{

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

void printVector(vec4 vIn) {
	printf("Vector is: %f, %f, %f\n",vIn.x,vIn.y,vIn.z);
}

// Find all triangles incident to this vertex

/* For HW6, we need to:
 * find the average of the normals of the triangles incident to the vertex. See Lecture 10, slide 53.
 */
void calculateVertexColors() {

	// Iterate thru all vertices, average the color info for their incident faces list
    for (auto& x: vectorFaceMapping) {
    		vec4 vertex = x.first;
    		std::vector<face> facesList = x.second;
    		for(auto& y : facesList) {
    			std::cout << "Vertex" << vertex << "incident on: " << y.faceIdx << std::endl;
    		}

    }


}


void calculateFaceColors() {
	for(int i = 0; i < smfFaces.size(); i++) {
		face currentFace = smfFaces.at(i);
		// Find the vertices it specifies and add them to vertices
		// Subtract 1 because SMF is 1-indexed
		// printf("Building face: %d - %f %f %f\n",i,currentFace.face1,currentFace.face2,currentFace.face3);
		vec4 vertex1 = currentFace.firstVertex; // smfVertices[index1 - 1];

		vec4 vertex2 = currentFace.secondVertex; // smfVertices[index2 - 1];

		vec4 vertex3 = currentFace.thirdVertex; // smfVertices[index3 - 1];

		int currentOffset = i * 3;

		points[currentOffset] = vertex1;
		points[currentOffset + 1] = vertex2;
		points[currentOffset + 2] = vertex3;


		colors[currentOffset] = currentFace.color;
		colors[currentOffset + 1] = currentFace.color;
		colors[currentOffset + 2] = currentFace.color;

	}
}

void calculateFaceColor(vec4 vertex1, vec4 vertex2, vec4 vertex3, face& currentFace) {
	// See p 272
			vec4 U = vertex2 - vertex1;
			vec4 V = vertex3 - vertex1;

			vec4 crossVector = cross(U,V);
			printf("Cross product ");
			printVector(crossVector);
			vec4 normalNormalized = normalize(crossVector);
			printf("Normalized vector ");
			printVector(normalNormalized);
			vec4 absNormalNormalized = vAbs(normalNormalized);
			printf("Absolute value vector ");
			printVector(absNormalNormalized);
			vec4 scaledAbsNormalNormalized = vScale(absNormalNormalized,50);
			printf("Scaled absolute value vector ");
			printVector(scaledAbsNormalNormalized);

			printf("Vertex 1 is: %f, %f, %f\n",vertex1.x,vertex1.y,vertex1.z);
			printf("Vertex 2 is: %f, %f, %f\n",vertex2.x,vertex2.y,vertex2.z);
			printf("Vertex 3 is: %f, %f, %f\n",vertex3.x,vertex3.y,vertex3.z);

			printf("Final Color is: %f, %f, %f, %f\n",scaledAbsNormalNormalized.x,scaledAbsNormalNormalized.y,scaledAbsNormalNormalized.z,scaledAbsNormalNormalized.w);

			currentFace.color = scaledAbsNormalNormalized;
}

int readSMF(char* fileName) {


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

					face f;
					f.faceIdx = numSmfFaces + 1; // faces are 1-indexed
					f.firstVertexIndex = int(b);
					f.secondVertexIndex = int(c);
					f.thirdVertexIndex = int(d);
					vec4 firstVertex = smfVertices[f.firstVertexIndex - 1];
					f.firstVertex = firstVertex;
					vec4 secondVertex = smfVertices[f.secondVertexIndex - 1];
					f.secondVertex = secondVertex;
					vec4 thirdVertex = smfVertices[f.thirdVertexIndex - 1];
					f.thirdVertex = thirdVertex;

					calculateFaceColor(firstVertex,secondVertex,thirdVertex,f);

					//vectorFaceMapping.con
					std::map<vec4,std::vector<face> >::iterator it;

					it = vectorFaceMapping.find(firstVertex);
					  if (it != vectorFaceMapping.end()) {
						  // List already exists
						  std::vector<face> incidentFacesList = it->second;
						  incidentFacesList.push_back(f);
					  }
					  else {
						  std::vector<face> incidentFacesList;
						  incidentFacesList.push_back(f);
						  vectorFaceMapping[firstVertex] = incidentFacesList;
					  }

					smfFaces.push_back(f);
					numSmfFaces++;
				}
			}

			NumVerticesUsed = numSmfFaces * 3;

			return numSmfFaces;
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
			calculateFaceColors();
			calculateVertexColors();
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
