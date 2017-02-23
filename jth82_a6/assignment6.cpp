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

class Face {
public:
	int faceIdx;
	int firstVertexIndex;
	int secondVertexIndex;
	int thirdVertexIndex;
	vec4 firstVertex;
	vec4 secondVertex;
	vec4 thirdVertex;
	vec4 normal;
};

bool debug = true;


std::map<int,std::vector<Face> > vertexFaceMapping;

mat4 TransformMatrix;
GLuint transformMatrix;

// Uniforms for lighting
// Light properties
color4 L_ambient, L_diffuse, L_specular;
point4 L_position = point4(0,-5,-5);

// Material properties
color4 M_reflect_ambient;
color4 M_reflect_diffuse;
color4 M_reflect_specular;

GLuint l_ambient, l_diffuse, l_specular, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular;

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
bool isPerspective = false;

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
std::vector<Face> smfFaces;

vec4 points[10000];
vec4 normals[10000];

vec3 ScaleFactors = vec3(1.0f, 1.0f, 1.0f);
vec3  RotationFactors = vec3(0.0f,0.0f,0.0f);
vec3  TranslationFactors = vec3(0.0f,0.0f,0.0f);

vec3 EyeVector = vec3(1.0f,1.0f,3.0f);

vec4 modelCentroid;

double bounding_box[6] = {-1.0, 1.0, -1.0, 1.0, -1.0, 1.0};


float Radius = 10.0; // Radius in degrees
float Phi = 57.2958; // Camera angle
int Theta = 90; // Longitude angle in degrees
float Height = 1;

float RadiusDelta = 1;
int Delta = 5;
float PhiDelta = 5;
float HeightDelta = .1;
float ParallelDelta = 2;

int mainWindow;

int w = 500;
int h = 500;
int border = 50;


vec4 defaultColor = vec4(.5,0,0,0);

GLint windowHeight, windowWidth;


float radians(float degrees) {
	return (M_PI * degrees) / 180;
}

void printVector(vec4 vIn) {
	printf("(%f, %f, %f)\n",vIn.x,vIn.y,vIn.z);
}

vec4 vProduct(vec4 a, vec4 b) {
	return vec4(a[0]*b[0],a[1]*b[1],a[2]*b[2],1.0);
}

vec4 vAbs(vec4 input) {
	vec4 absVec = vec4(std::abs(input.x),std::abs(input.y),
			std::abs(input.z),1.0);
	return absVec;
}

vec4 vScale(vec4 input, float scaleFactor) {
	vec4 scaleVec = vec4(scaleFactor * input.x,scaleFactor * input.y,scaleFactor * input.z,0.0);
	return scaleVec;
}


vec4 calculateModelCentroid() {
	vec4 sumOfAllPoints;
	for(int i = 0; i < NumVerticesUsed; i++) {
		if(debug) {
			printf("[Point]");
			printVector(points[i]);
		}
		sumOfAllPoints += points[i];
	}
	vec4 centroid = (sumOfAllPoints) / NumVerticesUsed;
	if(debug) {
		printf("Model centroid");
		printVector(centroid);
	}
	return centroid;
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

void calculateEyeVector2() {
//	Recall that the Cartesian coordinates of a point (X, Y , Z) defined in cylindrical coordinates (θ, R(adius), H(eight)) is
//	X = R * cos(θ)
//	Y = R * sin(θ)
//	Z = H
	float X, Y, Z;


	X = Radius * cos(radians(Theta));
	Y = Height;
	Z = Radius * sin(radians(Theta));

	EyeVector.x = X;
	EyeVector.y = Y;
	EyeVector.z = Z;

	printf("Eye Vector\n");
	printVector(EyeVector);




}



void
initMainWindow( void )
{


    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );

    // Print points and normals info
	if(debug) {
		for(int i = 0; i < NumVerticesUsed; i++) {
			vec4 currentPoint = points[i];
			printf("(Point)");
			printVector(currentPoint);
		}
		for(int i = 0; i < NumVerticesUsed; i++) {
			vec4 currentNormal = normals[i];
			printf("(Normal)");
			printVector(currentNormal);
		}
	}

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    //  glUseProgram( program );  // This is called in InitShader

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(normals)) );

    modelCentroid = calculateModelCentroid();

    transformMatrix = glGetUniformLocation(program, "transformMatrix");
    l_ambient = glGetUniformLocation(program, "l_ambient");
	l_diffuse = glGetUniformLocation(program, "l_diffuse");
	l_specular = glGetUniformLocation(program, "l_specular");
	m_reflect_ambient = glGetUniformLocation(program, "m_reflect_ambient");
	m_reflect_diffuse = glGetUniformLocation(program, "m_reflect_diffuse");
	m_reflect_specular = glGetUniformLocation(program, "m_reflect_specular");


    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );
    glFlush();

}



void
displayMainWindow( void )
{

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);

   mat4 Projection;
   if(isPerspective) {
	   Projection = Perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
   }
   else {
	   Projection = Ortho(left,right,bottom,top,near,far);

   }

   calculateEyeVector2();

   // Camera matrix
   mat4 View = LookAt(
	EyeVector,
	vec3(0,0,0),
	vec3(0,1,0)

       );

   // Move model to the origin mat4(1.0f);
   mat4 Model = Translate(modelCentroid);

   TransformMatrix = Projection * View * Model;

   glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );
   glUniform4fv(l_ambient, 1, L_ambient);
   glUniform4fv(l_diffuse, 1, L_diffuse);
   glUniform4fv(l_specular, 1, L_specular);

   glUniform4fv(m_reflect_ambient, 1, M_reflect_ambient);
   glUniform4fv(m_reflect_diffuse, 1, M_reflect_diffuse);
   glUniform4fv(m_reflect_specular, 1, M_reflect_specular);

   glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );

   glutSwapBuffers();

}


void reshape(GLsizei w, GLsizei h) {
	windowWidth = w;
	windowHeight = h;
	glViewport(0,0,windowWidth,windowHeight);
}

int max(int int1, int int2) {
	if(int1>int2) {
		return int1;
	}
	else {
		return int2;
	}
}

int min(int int1, int int2) {
	if(int1<int2) {
		return int1;
	}
	else {
		return int2;
	}
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
        	Radius += RadiusDelta;
        	if(Radius >= 360) {
        		Radius = 360;
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
        	Radius -= RadiusDelta;
        	if(Radius <= 1) {
        		Radius = 1;
        	}
    	}
    	else {
    		near -= ParallelDelta;
    		far -= ParallelDelta;

    	}


    	break;
    case '5' :
    	// Rotate counterclockwise
    	Theta += 5;
    	Theta = Theta % 360;
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
    	}
    	pressed = true;

    	break;
    case '6' :
    	Theta -= 5;
    	Theta = Theta % 360;
    	//Theta = max(Theta,50);
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
    	}
    	pressed = true;

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


	case 'q':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    }
    if(pressed) {

    	calculateEyeVector2();

        glutPostRedisplay();
    }


}


void idle() {


}


//----------------------------------------------------------------------------



// Find all triangles incident to this vertex

/* For HW6, we need to:
 * find the average of the normals of the triangles incident to the vertex. See Lecture 10, slide 53.
 */
vec4 calculateVertexNormal(int vertexIdx) {

    std::vector<Face> incidentFaces = vertexFaceMapping.at(vertexIdx);
    vec4 vertexNormal;
    vec4 incidentFacesColorsSum;

	int incidentFacesCount = 0;


	std::vector<Face>::iterator it;
	for(it=incidentFaces.begin() ; it < incidentFaces.end(); it++ ) {
		incidentFacesColorsSum += it->normal;
		incidentFacesCount++;
	}

	if(debug) {
		printf("Colors sum to: ");
		printVector(incidentFacesColorsSum);
		std::cout << "Incident faces count for vertex: " << vertexIdx << " is: " << incidentFacesCount << std::endl;
		std::cout << "Incident faces average: ";
	}


	vertexNormal = incidentFacesColorsSum / incidentFacesCount;

	if(debug) {
		printf("Average color is");
		printVector(vertexNormal);

	}

	return vertexNormal;




}


void populatePointsAndNormalsArrays() {
	for(int i = 0; i < smfFaces.size(); i++) {
		Face currentFace = smfFaces.at(i);

		vec4 vertex1 = currentFace.firstVertex;

		vec4 vertex2 = currentFace.secondVertex;

		vec4 vertex3 = currentFace.thirdVertex;

		int currentOffset = i * 3;

		points[currentOffset] = vertex1;
		points[currentOffset + 1] = vertex2;
		points[currentOffset + 2] = vertex3;

		// For now, do coloring in application. Will move to shader

		normals[currentOffset] = calculateVertexNormal(currentFace.firstVertexIndex);
		normals[currentOffset + 1] = calculateVertexNormal(currentFace.secondVertexIndex);
		normals[currentOffset + 2] = calculateVertexNormal(currentFace.thirdVertexIndex);



	}
}

void calculateFaceColor(vec4 vertex1, vec4 vertex2, vec4 vertex3, Face& currentFace) {
	// See p 272
			vec4 U = vertex2 - vertex1;
			vec4 V = vertex3 - vertex2;

			vec4 crossVector = cross(U,V);
			printf("Cross product ");
			printVector(crossVector);
			vec4 normalNormalized = normalize(crossVector);
			printf("Normalized vector ");
			printVector(normalNormalized);
			vec4 absNormalNormalized = vAbs(normalNormalized);
			printf("Absolute value vector ");
			printVector(absNormalNormalized);


			printf("Vertex 1 is: %f, %f, %f\n",vertex1.x,vertex1.y,vertex1.z);
			printf("Vertex 2 is: %f, %f, %f\n",vertex2.x,vertex2.y,vertex2.z);
			printf("Vertex 3 is: %f, %f, %f\n",vertex3.x,vertex3.y,vertex3.z);

			printf("Final Color is: %f, %f, %f, %f\n",absNormalNormalized.x,absNormalNormalized.y,absNormalNormalized.z,absNormalNormalized.w);

			currentFace.normal = absNormalNormalized;
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
					smfVertices[numSmfVertices] = vec4(b,c,d,1);
					numSmfVertices++;
				}
				else if(a == 'f') {

					Face f;
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

					vertexFaceMapping[f.firstVertexIndex].push_back(f);
					vertexFaceMapping[f.secondVertexIndex].push_back(f);
					vertexFaceMapping[f.thirdVertexIndex].push_back(f);

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
	//glEnable( GL_DEPTH_TEST );
    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
#endif
    glutInitWindowSize( 500, 500 );

    mainWindow = glutCreateWindow( "Assignment 6" );
#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif

    if(argc == 1) {
    			std::cout << "You must specify an SMF file" << std::endl;
    		}
    		else {
    			char* fileName = argv[1];
    			std::cout << "Filename is: " << argv[1] << std::endl;
    			readSMF(fileName);
    			populatePointsAndNormalsArrays();

    			std::cout << "Press: 1 - To increase camera height" << std::endl;
    			std::cout << "Press: 2 - To decrease camera height" << std::endl;
    			std::cout << "Press: 3 - To increase orbit radius" << std::endl;
    			std::cout << "Press: 4 - To decrease orbit radius" << std::endl;
    			std::cout << "Press: 5 - To increase cylinder angle (rotate camera)" << std::endl;
    			std::cout << "Press: 6 - To decrease cylinder angle (rotate counterclockwise)" << std::endl;
    			std::cout << "Press: 7 - To switch to perspective projection mode (default)" << std::endl;
    			std::cout << "Press: 8 - To switch to parallel projection mode" << std::endl;
    			std::cout << "Press: q - To exit the program" << std::endl;


    		}

	    glutInitWindowSize( w, h );
	    initMainWindow();
	    glutDisplayFunc( displayMainWindow );
	    glutKeyboardFunc( keyboard );
		glutIdleFunc(idle);






	    glutMainLoop();
	    return 0;
}
