// Make a 3D cube

#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

class Face {
public:
	int faceIdx;
	int firstVertexIndex;
	int secondVertexIndex;
	int thirdVertexIndex;
	vec3 firstVertex;
	vec3 secondVertex;
	vec3 thirdVertex;
	vec3 normal;
};

bool debug = true;


std::map<int,std::vector<Face> > vertexFaceMapping;

GLuint modelViewMatrix, projectionMatrix;

// Uniforms for lighting
// Light properties

vec3 P_ambient, P_diffuse, P_specular;

float M_shininess = 100;

GLuint p_ambient, p_diffuse, p_specular, l_position, e_position, m_shininess;

float eps = 0.001;

bool isPerspective = true;

GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;

float IsGouraud = .6; // >.5 is true, otherwise false

const int NumVertices = 10000; //(6 faces)(2 triangles/face)(3 vertices/triangle)

// Total number of vertices
int shape1VertexCount;

vec3 smfVertices[NumVertices];
std::vector<Face> smfFaces;

vec3 points[10000];
vec3 normals[10000];

vec3 EyeVector = vec3(1.0f,1.0f,10.0f);

vec3 modelCentroid;

float Radius, Theta, LightTheta, LightRadius, Height, LightHeight;

float RadiusDelta = 1.0f;
float Delta = 5.0f;
float HeightDelta = 1.0f;
float ParallelDelta = 2.0f;
float ThetaDelta = 5.0f;

int mainWindow;

int w = 500;
int h = 500;

float radians(float degrees) {
	return (M_PI * degrees) / 180;
}

void printVector(vec3 vIn) {
	printf("(%f, %f, %f)\n",vIn.x,vIn.y,vIn.z);
}

vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}


vec3 calculateModelCentroid() {
	vec3 sumOfAllPoints;
	for(int i = 0; i < shape1VertexCount; i++) {
		sumOfAllPoints += points[i];
	}
	vec3 centroid = (sumOfAllPoints) / shape1VertexCount;
	return centroid;
}

void setDefaultViewParams() {
	// Don't want to wash out with ambient light

	// Base color is red
	P_ambient = vProduct(vec3(0.15, .15, .15),vec3(1,1,1));
	P_diffuse = vProduct(vec3(0.6, .6, .6),vec3(.5,0,0));
	P_specular = vProduct(vec3(0.25, .25, .25),vec3(1,1,1));
	M_shininess = 100;
	Radius = 3.0;
	Height = 0.0f;
	Theta = 5.0f;
	LightTheta = Theta;
	LightRadius = Radius;
	LightHeight = Height;

	RadiusDelta = 1;

}

vec3 calculateEyeVector() {


	float X, Y, Z;

	X = Radius * cos(radians(Theta));
	Y = Height;
	Z = Radius * sin(radians(Theta));

	return vec3(X,Y,Z);
}

vec3 calculateLightVector() {

	float X, Y, Z;

	X = LightRadius * cos(radians(LightTheta));
	Y = LightHeight;
	Z = LightRadius * sin(radians(LightTheta)) + 1; // Ensure light is a bit behind camera

	return vec3(X,Y,Z);

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
       sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(normals)) );

    modelCentroid = calculateModelCentroid();

    projectionMatrix = glGetUniformLocation(program, "Projection");
    modelViewMatrix = glGetUniformLocation(program, "ModelView");
    p_ambient = glGetUniformLocation(program, "AmbientProduct");
	p_diffuse = glGetUniformLocation(program, "DiffuseProduct");
	p_specular = glGetUniformLocation(program, "SpecularProduct");
	l_position = glGetUniformLocation(program, "LightPosition");
	e_position = glGetUniformLocation(program, "EyePosition");
	m_shininess = glGetUniformLocation(program, "Shininess");

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // black background

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glFlush();

}



void
displayMainWindow( void )
{

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   mat4 Projection;
   if(isPerspective) {
	   Projection = Perspective(45.0f, 1.0f, 0.1f, 100.0f);
   }
   else {
	   Projection = Ortho(left,right,bottom,top,-0.001f,100.f);
   }


   vec3 eyePos = calculateEyeVector();
   vec4 lightPos = calculateLightVector();

   vec3 modelCentroid = calculateModelCentroid();

   // Look at model centroid
   mat4 model_view = LookAt(
	eyePos,
	modelCentroid,
	vec4(0,1,0,1)
       );

   mat4 ModelViewMatrix = model_view;

   glUniformMatrix4fv( projectionMatrix, 1, GL_TRUE, Projection );
   glUniformMatrix4fv( modelViewMatrix, 1, GL_TRUE, ModelViewMatrix );

   glUniform3fv(p_ambient, 1, P_ambient);
   glUniform3fv(p_diffuse, 1, P_diffuse);
   glUniform3fv(p_specular, 1, P_specular);
   glUniform4fv(l_position, 1, lightPos);
   glUniform3fv(e_position,1,eyePos);
   glUniform1f(m_shininess,M_shininess);

   glDrawArrays( GL_TRIANGLES, 0, shape1VertexCount );

   glutSwapBuffers();

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

    switch ( key ) {


    case '1' :
    	// Increase Height
    	Height += HeightDelta;
    	LightHeight += HeightDelta;
    	break;
    case '2' :
    	// Decrease height
    	Height -= HeightDelta;
    	LightHeight -= HeightDelta;
    	break;

    case '3' :
    	// Increase orbit radius / distance of camera
		Radius += RadiusDelta;
    	LightRadius += RadiusDelta;

    	if(debug) {
    		printf("Radius is: %f\n",Radius);
    		printf("LightRadius is: %f\n",LightRadius);
    	}

    	if(!isPerspective) {
			near += ParallelDelta;
			far += ParallelDelta;
    	}


    	break;
    case '4' :
		Radius -= RadiusDelta;
    	LightRadius -= RadiusDelta;

    	// Clamp to 1; a negative radius doesn't make sense

    	if(Radius <= 1 + eps || LightRadius <= 1 + eps) {
    		Radius = LightRadius = 1;
    	}

    	if(debug) {
    		printf("Radius is: %f\n",Radius);
    		printf("LightRadius is: %f\n",LightRadius);
    	}

    	if(!isPerspective) {
			near -= ParallelDelta;
			far -= ParallelDelta;
    	}

    	break;

    case '5' :
    	// Rotate counterclockwise
    	Theta += ThetaDelta;
    	LightTheta += ThetaDelta;
    	Theta = min(Theta,360);
    	LightTheta = min(LightTheta,360);
    	if(debug) {
    		printf("Theta is: %f\n",Theta);
    		printf("LightRadius is: %f\n",LightTheta);

    	}

    	break;
    case '6' :
    	Theta -= ThetaDelta;
    	LightTheta -= ThetaDelta;
    	Theta = max(Theta,5.0);
    	LightTheta = max(LightTheta,5.0);
    	if(debug) {
    		printf("Theta is: %f\n",Theta);
    		printf("LightRadius is: %f\n",LightTheta);
    	}
    	break;

    case '7' :
    	// Set perspective projection
    	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    	isPerspective = true;
    	break;
    case '8' :
    	isPerspective = false;
    	break;
    case 'g' :
    	IsGouraud = .6;
    	if(debug) {
    		printf("Gouraud shading mode\n");
    	}
    	break;
    case 'p' :
    	IsGouraud = .4;
    	if(debug) {
    		printf("Phong shading mode\n");
    	}
    	break;
    case 'a' :
    	if(debug) {
    		printf("Material 1 selected");
    	}
    	/*
    	L_ambient = vec3(1.0, 1.0, 1.0);
    	L_diffuse = vec3(1.0, 1.0, 1.0);
    	L_specular = vec3(1.0, .5, .5);
    	M_reflect_ambient = vec3(0.7, .3, .7);
    	M_reflect_diffuse = vec3(0.2, .6, .2);
    	M_reflect_specular = vec3(0.1, .1, .1); */
    	M_shininess = 500;

    	break;

    case 's' :
    	if(debug) {
    		printf("Material 2 selected");
    	}
    	/*
    	L_ambient = vec3(1.0, 1.0, 1.0);
    	L_diffuse = vec3(1.0, 1.0, 1.0);
    	L_specular = vec3(1.0, .5, .5);
    	M_reflect_ambient = vec3(0.3, .7, .7);
    	M_reflect_diffuse = vec3(0.2, .6, .2);
    	M_reflect_specular = vec3(0.1, .1, .1); */
    	M_shininess = 100;


    	break;

    case 'd' :
    	if(debug) {
    		printf("Material 3 selected");
    	}
    	/*
    	L_ambient = vec3(1.0, 1.0, 1.0);
    	L_diffuse = vec3(1.0, 1.0, 1.0);
    	L_specular = vec3(1.0, .5, .5);
    	M_reflect_ambient = vec3(0.1, .1, .8);
    	M_reflect_diffuse = vec3(0.1, .1, .8);
    	M_reflect_specular = vec3(0.1, .1, .1); */
    	M_shininess = 1000;

    	break;

	case 'x':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    case 'z':
    	setDefaultViewParams();
    	break;

    }


	glutPostRedisplay();

}


void idle() {


}


//----------------------------------------------------------------------------



// Find all triangles incident to this vertex

/* For HW6, we need to:
 * find the average of the normals of the triangles incident to the vertex. See Lecture 10, slide 53.
 */
vec3 calculateVertexNormal(int vertexIdx) {

    std::vector<Face> incidentFaces = vertexFaceMapping.at(vertexIdx);
    vec3 vertexNormal;
    vec3 incidentFacesColorsSum;

	int incidentFacesCount = 0;


	std::vector<Face>::iterator it;
	for(it=incidentFaces.begin() ; it < incidentFaces.end(); it++ ) {
		printf("Normal for face %d incident to vertex %d is: ",it->faceIdx,vertexIdx);
		printVector(it->normal);
		if(std::isnan(it->normal.x) || std::isnan(it->normal.y) || std::isnan(it->normal.z)) {
			// We weren't able to calculate the normal. Set it to default color
			printf("NaN\n");
			printVector(it->normal);
			exit(1);
		}
		printVector(incidentFacesColorsSum);
		incidentFacesColorsSum += it->normal;
		printf("New sum is: ");
		printVector(incidentFacesColorsSum);
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

vec3 calculateNormal(vec3 vertex1, vec3 vertex2, vec3 vertex3) {
	vec3 U = vertex2 - vertex1;
	vec3 V = vertex3 - vertex2;
	return normalize(cross(U,V));
}


void populatePointsAndNormalsArrays() {
	for(uint i = 0; i < smfFaces.size(); i++) {
		Face currentFace = smfFaces.at(i);

		vec3 vertex1 = currentFace.firstVertex;

		vec3 vertex2 = currentFace.secondVertex;

		vec3 vertex3 = currentFace.thirdVertex;

		int currentOffset = i * 3;

		points[currentOffset] = vertex1;
		points[currentOffset + 1] = vertex2;
		points[currentOffset + 2] = vertex3;

		vec3 faceNormal = calculateNormal(vertex1,vertex2,vertex3);

		normals[currentOffset] = faceNormal; // calculateVertexNormal(currentFace.firstVertexIndex);
		normals[currentOffset + 1] = faceNormal; // calculateVertexNormal(currentFace.secondVertexIndex);
		normals[currentOffset + 2] = faceNormal; // calculateVertexNormal(currentFace.thirdVertexIndex);

	}
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
					smfVertices[numSmfVertices] = vec3(b,c,d);
					numSmfVertices++;
				}
				else if(a == 'f') {

					Face f;
					f.faceIdx = numSmfFaces + 1; // faces are 1-indexed
					f.firstVertexIndex = int(b);
					f.secondVertexIndex = int(c);
					f.thirdVertexIndex = int(d);
					vec3 firstVertex = smfVertices[f.firstVertexIndex - 1];
					f.firstVertex = firstVertex;
					vec3 secondVertex = smfVertices[f.secondVertexIndex - 1];
					f.secondVertex = secondVertex;
					vec3 thirdVertex = smfVertices[f.thirdVertexIndex - 1];
					f.thirdVertex = thirdVertex;

					vec3 normal = calculateNormal(firstVertex,secondVertex,thirdVertex);
					f.normal = normal;

					vertexFaceMapping[f.firstVertexIndex].push_back(f);
					vertexFaceMapping[f.secondVertexIndex].push_back(f);
					vertexFaceMapping[f.thirdVertexIndex].push_back(f);

					smfFaces.push_back(f);
					numSmfFaces++;
				}
			}

			shape1VertexCount = numSmfFaces * 3;

			return numSmfFaces;
}

int
main( int argc, char **argv )
{
	//glEnable( GL_DEPTH_TEST );
    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
#endif
    glutInitWindowSize( w, h );

    glutCreateWindow( "Assignment 6" );
#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif

    char* fileName;
    if(argc == 1) {
    	fileName = "bound-bunny_200.smf";
	}
	else {
		fileName = argv[1];
	}
	readSMF(fileName);
	setDefaultViewParams();
	populatePointsAndNormalsArrays();

	std::cout << "Press: 1 - To increase camera height" << std::endl;
	std::cout << "Press: 2 - To decrease camera height" << std::endl;
	std::cout << "Press: q - To increase light height" << std::endl;
	std::cout << "Press: w - To decrease light height" << std::endl;
	std::cout << "Press: 3 - To increase orbit radius" << std::endl;
	std::cout << "Press: 4 - To decrease orbit radius" << std::endl;
	std::cout << "Press: e - To increase light radius" << std::endl;
	std::cout << "Press: r - To decrease light radius" << std::endl;
	std::cout << "Press: 5 - To increase camera angle (rotate camera)" << std::endl;
	std::cout << "Press: 6 - To decrease camera angle (rotate counterclockwise)" << std::endl;
	std::cout << "Press: t - To increase light angle (rotate camera)" << std::endl;
	std::cout << "Press: y - To decrease light angle (rotate counterclockwise)" << std::endl;
	std::cout << "Press: 7 - To enable perspective projection mode" << std::endl;
	std::cout << "Press: 8 - To enable parallel projection mode (default)" << std::endl;
	std::cout << "Press: g - To enable Gouraud shading" << std::endl;
	std::cout << "Press: p - To enable Phong shading" << std::endl;
	std::cout << "Press: a - To select material 1 (reflects green, highly specular)" << std::endl;
	std::cout << "Press: s - To select material 2 (reflects dark blue, low specular)" << std::endl;
	std::cout << "Press: d - To select material 3 (reflects dark green, medium specular)" << std::endl;
	std::cout << "Press: z - To reset the points" << std::endl;
	std::cout << "Press: x - To exit the program" << std::endl;


	initMainWindow();

	glutDisplayFunc( displayMainWindow );
	glutKeyboardFunc( keyboard );

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}
