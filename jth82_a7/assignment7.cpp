#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int N = 20;
const int uRange = N;
const int vRange = N;

vec4 vertices[10000];
vec4 patch[4][4];
int totalRead = 0;

vec4 interpolatedPoints[N][N];

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

point4 L_position = point4(0,5,10,1);

// Material properties

vec4 materialAmbientLightProperties[3];
vec4 materialDiffuseLightProperties[3];
vec4 materialSpecularLightProperties[3];

vec4 materialAmbientReflectionProperties[3];
vec4 materialDiffuseReflectionProperties[3];
vec4 materialSpecularReflectionProperties[3];


color4 L_ambient = vec4(1.0,1.0,1.0,1.0);
color4 L_diffuse = vec4(1.0,1.0,1.0,0.5);
color4 L_specular = vec4(.5,.5,.5,1);

color4 M_reflect_ambient = vec4(0.5,1,0.0,1.0);
color4 M_reflect_diffuse = vec4(0.5,1,0.0,1.0);
color4 M_reflect_specular = vec4(1.0,1,1.0,1.0);

float M_shininess = 10;

GLuint l_ambient, l_diffuse, l_specular, l_position, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, m_shininess;
GLuint cameraPosition;
GLuint isGouraud;
GLuint flatShading;

// Projection matrix : 45° Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
bool isPerspective = true;

// For Ortho coordinates
GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;

float IsGouraud = .6; // >.5 is true, otherwise false
float FlatShading = .6; // >.5 is true, otherwise false


const int NumVertices = 10000;
int NumVerticesUsed = 24;

vec4 smfVertices[NumVertices];
std::vector<Face> smfFaces;

vec4 points[10000];
vec4 normals[10000];

vec4 EyeVector = vec4(1.0f,1.0f,10.0f,1.0f);

vec4 modelCentroid;

float Radius = 18.0;
int Theta = 90; // Longitude angle in degrees
int LightTheta = 190;
int LightRadius = -91;
float Height, LightHeight = 3;

float RadiusDelta = 1;
int Delta = 5;
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

float getBernsteinFactor(float u, int sub) {
	float uu = 1-u;
	float val1 = uu * uu * uu;
	float val2 = 3 * u * uu * uu;
	float val3 = 3 * u * u * uu;
	float val4 = u * u * u;
	if(sub == 1) {
		return val1;
	}
	else if(sub == 2) {
		return val2;
	}
	else if(sub == 3) {
		return val3;
	}
	else {
		return val4;
	}
}

void calcPatchPoints() {

	for(int u = 0; u < uRange; u++) {

		float uParam = (float) u / (float)uRange;

		for(int v = 0; v < vRange; v++) {

			float vParam = (float) v / (float)vRange ;

			vec4 pointSum = vec4(0,0,0,0);

			for(int i = 0; i < 4; i++) {

				float bernsteinForU = getBernsteinFactor(uParam,i);

				for(int j = 0; j < 4; j++) {

					float bernsteinForV = getBernsteinFactor(vParam,j);

					float controlX = patch[i][j].x;
					float controlY = patch[i][j].y;
					float controlZ = patch[i][j].z;
					vec4 controlPoint = vec4(controlX,controlY,controlZ,0);
					float weight = bernsteinForU * bernsteinForV;
					pointSum += weight * controlPoint;
				}
			}

			interpolatedPoints[u][v] = pointSum;
		} // end v loop

	} // end u loop


} // end method


// Used for SMF-format printing
void printVertex(vec4 vertex, std::ofstream& of) {
	of << "v" << " " << (double) vertex.x << " " << (double) vertex.y << " " << (double) vertex.z << "\n";
}

void printFace(int vertex1, int vertex2, int vertex3, std::ofstream& of) {
	of << "f" << " " << vertex1 << " " << vertex2<< " " << vertex3 << "\n";
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
	EyeVector.w = 0;

	X = LightRadius * cos(radians(LightTheta));
	Y = LightHeight;
	Z = LightRadius * sin(radians(LightTheta));

	L_position.x = X;
	L_position.y = Y;
	L_position.z = Z;
	L_position.w = 0;

	if(debug) {
		printf("Eye Vector\n");
		printVector(EyeVector);
	}


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

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_TRUE, 0,
                           BUFFER_OFFSET(sizeof(normals)) );

    modelCentroid = calculateModelCentroid();

    transformMatrix = glGetUniformLocation(program, "transformMatrix");
    l_ambient = glGetUniformLocation(program, "l_ambient");
	l_diffuse = glGetUniformLocation(program, "l_diffuse");
	l_specular = glGetUniformLocation(program, "l_specular");
	l_position = glGetUniformLocation(program, "l_position");
	m_reflect_ambient = glGetUniformLocation(program, "m_reflect_ambient");
	m_reflect_diffuse = glGetUniformLocation(program, "m_reflect_diffuse");
	m_reflect_specular = glGetUniformLocation(program, "m_reflect_specular");
	m_shininess = glGetUniformLocation(program, "m_shininess");
	cameraPosition = glGetUniformLocation(program, "cameraPosition");
	isGouraud = glGetUniformLocation(program, "isGouraud");
	flatShading = glGetUniformLocation(program, "flatShading");

    glClearColor( 0.2, 0.2, 0.2, 0.2 ); // grey background



    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glPointSize(20.0f);
    // glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );
    glDrawArrays( GL_POINTS, 0, NumVerticesUsed );

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

   calculateEyeVector2();

   // Camera matrix
   mat4 View = LookAt(
	EyeVector,
	vec4(0,0,0,1),
	vec4(0,1,0,0)

       );

   // Move model to the origin mat4(1.0f);
   mat4 Model = Translate(-1 * modelCentroid);

   TransformMatrix = Projection * View * Model;

   glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );
   glUniform4fv(l_ambient, 1, L_ambient);
   glUniform4fv(l_diffuse, 1, L_diffuse);
   glUniform4fv(l_specular, 1, L_specular);
   glUniform4fv(l_position, 1, L_position);


   glUniform4fv(m_reflect_ambient, 1, M_reflect_ambient);
   glUniform4fv(m_reflect_diffuse, 1, M_reflect_diffuse);
   glUniform4fv(m_reflect_specular, 1, M_reflect_specular);
   glUniform1f(m_shininess,M_shininess);
   glUniform4fv(m_reflect_specular, 1, M_reflect_specular);
   glUniform4fv(cameraPosition,1,EyeVector);
   glUniform1f(m_shininess,M_shininess);
   glUniform1f(isGouraud,IsGouraud);
   glUniform1f(flatShading,FlatShading);

   glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );
   glDrawArrays( GL_POINTS, 0, NumVerticesUsed );

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
	// Define six keys for increasing and decreasing the X,Y,Z components of the current transformation.
	// The cube should only be transformed with each key stroke.
	bool pressed = false;

	// We either manipulate ScaleFactor, RotationFactor, or TranslateFactor, depending on current operation

    switch ( key ) {


    case '1' :
    	// Increase Height
    	Height += HeightDelta;
    	break;
    case '2' :
    	// Decrease height
    	Height -= HeightDelta;
    	break;
    case 'q' :
    	// Increase Height
    	LightHeight += HeightDelta;
    	break;
    case 'w' :
    	// Decrease height
    	LightHeight -= HeightDelta;
    	break;
    case '3' :
    	// Increase orbit radius / distance of camera
		Radius += RadiusDelta;
		if(Radius >= 360) {
			Radius = 360;
		}

		near += ParallelDelta;
		far += ParallelDelta;


    	break;
    case '4' :
		Radius -= RadiusDelta;
		if(Radius <= 1) {
			Radius = 1;
		}

		near -= ParallelDelta;
		far -= ParallelDelta;

    	break;
    case 'e' :
		// Increase orbit radius / distance of light
    	LightRadius += RadiusDelta;
    	if(debug) {
    		printf("LightRadius is: %d\n",LightRadius);
    	}
    	pressed = true;
        break;

	case 'r' :
		// Increase orbit radius / distance of light
    	LightRadius -= RadiusDelta;
    	if(debug) {
    		printf("LightRadius is: %d\n",LightRadius);
    	}
    	pressed = true;
        break;


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
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
    	}
    	pressed = true;

    	break;
    case 't' :
    	// Rotate counterclockwise
    	LightTheta += 5;
    	LightTheta = LightTheta % 360;
    	if(debug) {
    		printf("LightTheta is: %d\n",LightTheta);
    	}
    	pressed = true;

    	break;
    case 'y' :
    	LightTheta -= 5;
    	LightTheta = LightTheta % 360;
    	if(debug) {
    		printf("LightTheta is: %d\n",LightTheta);
    	}

    	break;
    case '7' :
    	// Set perspective projection
    	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    	isPerspective = true;

    	pressed = true;
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
    case 'f' :
    	FlatShading = .6;
    	if(debug) {
    		printf("Turned on flat shading\n");
    	}
    	break;
    case 'v' :
    	FlatShading = .4;
    	if(debug) {
    		printf("Turned off flat shading\n");
    	}
    	break;
    case 'a' :
    	if(debug) {
    		printf("Material 1 selected");
    	}

    	L_ambient = vec4(1.0,1.0,1.0,1.0);
    	L_diffuse = vec4(1.0,1.0,1.0,0.5);
    	L_specular = vec4(.5,.5,.5,1);

    	M_reflect_ambient = vec4(0.5,1,0.0,1.0);
    	M_reflect_diffuse = vec4(0.5,1,0.0,1.0);
    	M_reflect_specular = vec4(1.0,1,1.0,1.0);

    	break;

    case 's' :
    	if(debug) {
    		printf("Material 2 selected");
    	}
    	L_ambient = vec4(1.0,1.0,1.0,1.0);
    	L_diffuse = vec4(1.0,1.0,1.0,0.5);
    	L_specular = vec4(.5,.5,.5,1);

    	M_reflect_ambient = vec4(1.0,0,1.0,1.0);
    	M_reflect_diffuse = vec4(1.0,.4,0.0,1.0);
    	M_reflect_specular = vec4(0.0,.4,.4,1.0);
    	pressed = true;

    	break;

    case 'd' :
    	if(debug) {
    		printf("Material 3 selected");
    	}
    	L_ambient = vec4(1.0,1.0,1.0,1.0);
    	L_diffuse = vec4(1.0,1.0,1.0,0.5);
    	L_specular = vec4(.5,.5,.5,1);

    	M_reflect_ambient = vec4(0.0,1,0.0,1.0);
    	M_reflect_diffuse = vec4(1.0,1,0.0,1.0);
    	M_reflect_specular = vec4(1.0,1,.5,1.0);
    	pressed = true;

    	break;

    case 'z':
    	if(debug) {
    		printf("Reset all values\n");
    	}

    	L_ambient = vec4(1.0,1,0.0,1);
    	L_diffuse = vec4(0.0,1,0.0,1);
    	L_specular = vec4(1.0,1.0,1.0,1);

    	M_reflect_ambient = vec4(0.0,1,0.0,1.0);
    	M_reflect_diffuse = vec4(0.0,1,0.0,1.0);
    	M_reflect_specular = vec4(1.0,1,1.0,1.0);

    	FlatShading = 0.6;

    	Radius = 18.0;
    	Theta = 90; // Longitude angle in degrees
    	LightTheta = 190;
    	LightRadius = -91;
    	Height = 3;
		LightHeight = 3;

    	RadiusDelta = 1;
    	Delta = 5;
    	HeightDelta = .1;
    	ParallelDelta = 2;

    	break;


	case 'x':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    }

	calculateEyeVector2();
	glutPostRedisplay();

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
		printf("Normal for face %d incident to vertex %d is: ",it->faceIdx,vertexIdx);
		printVector(it->normal);
		if(std::isnan(it->normal.x)) {
			// We weren't able to calculate the normal. Set it to default color
			it->normal = normalize(defaultColor);
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

		normals[currentOffset] = calculateVertexNormal(currentFace.firstVertexIndex);
		normals[currentOffset + 1] = calculateVertexNormal(currentFace.secondVertexIndex);
		normals[currentOffset + 2] = calculateVertexNormal(currentFace.thirdVertexIndex);

	}
}

void calculateFaceNormal(vec4 vertex1, vec4 vertex2, vec4 vertex3, Face& currentFace) {
		// See p 272
		vec4 U = vertex2 - vertex1;
		vec4 V = vertex3 - vertex2;

		vec4 crossVector = cross(U,V);

		double customLength = sqrt(crossVector.x*crossVector.x+crossVector.y*crossVector.y+crossVector.z*crossVector.z);

		vec4 customNormal = crossVector / customLength;

		vec4 absCustomNormal = vAbs(customNormal);

		if(debug) {
			printf("Cross product ");
			printVector(crossVector);
			printf("Normalized vector ");
			printf("Absolute value vector ");

			printf("Vertex 1 is: %f, %f, %f\n",vertex1.x,vertex1.y,vertex1.z);
			printf("Vertex 2 is: %f, %f, %f\n",vertex2.x,vertex2.y,vertex2.z);
			printf("Vertex 3 is: %f, %f, %f\n",vertex3.x,vertex3.y,vertex3.z);

			printf("Final Color is: %f, %f, %f, %f\n",absCustomNormal.x,absCustomNormal.y,absCustomNormal.z,absCustomNormal.w);
		}

		currentFace.normal = absCustomNormal;
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

					calculateFaceNormal(firstVertex,secondVertex,thirdVertex,f);

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

int readPatchFile(char* fileName) {

	// Read in the patch file
	std::ifstream infile(fileName);

	float a, b, c;
	int numVertices = 0;
	while (infile >> a >> b >> c)
	{
		vec4 vertex = vec4(a,b,c,1.0);
		vertices[numVertices++] = vertex;
	}

	return numVertices;

	infile.close();
}


void readPatchFileAndPrintSMF(char* patchName, char* smfName) {

	readPatchFile(patchName);

	// Convert the 16 control vertices into a 4 by 4 array
	int idx = 0;
	for(int i=0; i < 4; i++) {
		for(int j=0; j<4; j++) {
			vec4 vertex = vertices[idx++];
			patch[i][j] = vertex;
		}
	}

	// Print the dynamically loaded patch array
	for(int i=0; i < 4; i++) {
		for(int j=0;j<4;j++) {
			vec4 vertex = patch[i][j];
		}
	}

	calcPatchPoints();

	int vertexNum = 1;

	std::ofstream ofs (smfName, std::ofstream::out);


	// Write out all interpolated points in SMF format
	for(int i = 0; i <= N - 1; i++) {

			for(int j=0; j <= N - 1; j++) {

				// First triangle
				vec4 vertex1 =  patch[i][j]; // 1
				vec4 vertex2 = patch[i+1][j]; // 2
				vec4 vertex3 = patch[j+1][i]; // 3

				// Second triangle
				vec4 vertex4 = patch[i+1][j]; // 4 == 2, not 1
				vec4 vertex5 = patch[j+1][i+1]; // 5
				vec4 vertex6 = patch[i][j+1]; // 6

				printVertex(vertex1,ofs);
				printVertex(vertex2,ofs);
				printVertex(vertex3,ofs);

				int vertexNum1 = vertexNum;
				int vertexNum2 = vertexNum + 1;
				int vertexNum3 = vertexNum + 2;

				// Print face info for triangle 1
				printFace(vertexNum1,vertexNum2,vertexNum3, ofs);

				vertexNum += 3;

				int vertexNum4 = vertexNum;
				int vertexNum5 = vertexNum + 1;
				int vertexNum6 = vertexNum + 2;

				printVertex(vertex4,ofs);
				printVertex(vertex5,ofs);
				printVertex(vertex6,ofs);
				// Print face info for triangle 2
				printFace(vertexNum4,vertexNum5,vertexNum6, ofs);

				vertexNum += 3;

			}
		}

	  ofs.close();
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
    glutInitWindowSize( 500, 500 );

    mainWindow = glutCreateWindow( "Assignment 7" );
#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif


	char* patchFileName = "patchPoints.txt";
	char* smfName = "interpolatedBezier.smf";

	readPatchFileAndPrintSMF(patchFileName,smfName);

	// Convert the 16 control vertices into a 4 by 4 array
	int idx = 0;
	for(int i=0; i < 4; i++) {
		for(int j=0; j<4; j++) {
			vec4 vertex = vertices[idx++];
			patch[i][j] = vertex;
		}
	}

	// Interpolate as desired and write out an SMF

	readSMF(smfName);

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
	std::cout << "Press: f - To enable flat shading (default)" << std::endl;
	std::cout << "Press: v - To disable flat shading" << std::endl;
	std::cout << "Press: a - To select material 1 (reflects green, highly specular)" << std::endl;
	std::cout << "Press: s - To select material 2 (reflects dark blue, low specular)" << std::endl;
	std::cout << "Press: d - To select material 3 (reflects dark green, medium specular)" << std::endl;

	std::cout << "Press: x - To exit the program" << std::endl;


	initMainWindow();

	glutDisplayFunc( displayMainWindow );
	glutKeyboardFunc( keyboard );

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}
