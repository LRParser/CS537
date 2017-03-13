#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>

int selectedPointIdx = 0;

bool debug = false;

GLuint modelMatrix, viewMatrix, projectionMatrix;

mat4 TransformMatrix;
GLuint transformMatrix;

float eps = .001;

// Uniforms for lighting

vec3 L_position = vec3(0,5,10);
vec3 materialAmbientLightProperties[3];
vec3 materialDiffuseLightProperties[3];
vec3 materialSpecularLightProperties[3];
vec3 materialAmbientReflectionProperties[3];
vec3 materialDiffuseReflectionProperties[3];
vec3 materialSpecularReflectionProperties[3];

vec3 L_ambient = vec3(1.0,1.0,1.0);
vec3 L_diffuse = vec3(1.0,1.0,1.0);
vec3 L_specular = vec3(1.0,.5,.5);
vec3 M_reflect_ambient = vec3(0.7,.3,.7);
vec3 M_reflect_diffuse = vec3(0.2,.6,.2);
vec3 M_reflect_specular = vec3(0.1,.1,.1);

float M_shininess = 50;

GLuint l_ambient, l_diffuse, l_specular, l_position, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, m_shininess;
GLuint eyePosition;

bool isPerspective = true;

GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;

// Is set in tesellateInterpolatedPoints, and used in initMainWindow
int totalNumVertices;

const int defaultSize = 100000;
vec3 points[defaultSize];
vec3 normals[defaultSize];
vec3 controlVertices[defaultSize];
vec3 patch[4][4];

int uRange = 10;
int vRange = 10;
vec3 interpolatedPoints[51][51];

vec3 lineBufferData[6];

vec3 EyeVector = vec3(1.0f,1.0f,10.0f);

vec3 modelCentroid;

float Radius = 18.0;
int Theta = 0; // Longitude angle in degrees
int LightTheta = Theta;
int LightRadius = Radius;
float Height = 3;
float LightHeight = Height;

float RadiusDelta = 1;
int Delta = 5;
float HeightDelta = .1;
float ParallelDelta = 2;

int mainWindow;

int w = 500;
int h = 500;
int border = 50;


float radians(float degrees) {
	return (M_PI * degrees) / 180;
}

void printVector(vec3 vIn) {
	printf("(%f, %f, %f)\n",vIn.x,vIn.y,vIn.z);
}

float getBernsteinFactor(float u, int sub) {
	float uu = 1-u;
	if(sub == 0) {
		return uu * uu * uu;
	}
	else if(sub == 1) {
		return 3 * u * uu * uu;
	}
	else if(sub == 2) {
		return 3 * u * u * uu;
	}
	else {
		return u * u * u;
	}
}

// For the given desired interpolation range
void interpolatePatch(int uRange, int vRange) {

	printf("Interpolate u to: %d and v to %d\n",uRange,vRange);

	for(int u = 0; u <= uRange; u++) {

		float uParam = (float) u / (float)uRange;
		printf("uParam is %f\n",uParam);

		for(int v = 0; v <= vRange; v++) {

			float vParam = (float) v / (float)vRange ;
			printf("vParam is %f\n",vParam);

			vec3 pointSum = vec3(0,0,0);

			for(int i = 0; i < 4; i++) {

				float bernsteinForU = getBernsteinFactor(uParam,i);

				for(int j = 0; j < 4; j++) {

					float bernsteinForV = getBernsteinFactor(vParam,j);

					float controlX = patch[j][i].x;
					float controlY = patch[j][i].y;
					float controlZ = patch[j][i].z;
					vec3 controlPoint = vec3(controlX,controlY,controlZ);
					float weight = bernsteinForU * bernsteinForV;
					pointSum += weight * controlPoint;
				}
			}

			interpolatedPoints[u][v] = pointSum;
		} // end v loop

	} // end u loop

} // end method

void printVertex(vec3 vertex) {
	printf("v %f %f %f\n",vertex.x,vertex.y,vertex.z);
}

vec3 vAbs(vec3 input) {
	vec3 absVec = vec3(std::abs(input.x),std::abs(input.y),
			std::abs(input.z));
	return absVec;
}

vec3 calculateModelCentroid() {
	vec3 sumOfAllPoints;
	for(int i = 0; i < totalNumVertices; i++) {
		sumOfAllPoints += points[i];
	}
	vec3 centroid = (sumOfAllPoints) / totalNumVertices;
	return centroid;
}

void calculateEyeVector() {

	float X, Y, Z;

	X = Radius * cos(radians(Theta));
	Y = Height;
	Z = Radius * sin(radians(Theta));

	EyeVector.x = X;
	EyeVector.y = Y;
	EyeVector.z = Z;

	X = LightRadius * cos(radians(LightTheta));
	Y = LightHeight;
	Z = LightRadius * sin(radians(LightTheta));

	L_position.x = X;
	L_position.y = Y;
	L_position.z = Z;

}

void setLineBufferData() {
    // X axis
    lineBufferData[0] = vec3(0,0,0);
    lineBufferData[1] = vec3(10,0,0);

    // Y axis
    lineBufferData[2] = vec3(0,0,0);
    lineBufferData[3] = vec3(0,10,0);

    // Z axis
    lineBufferData[4] = vec3(0,0,0);
    lineBufferData[5] = vec3(0,0,10);
}

void printPointsAndNormals() {
	for(int i = 0; i < totalNumVertices; i++) {
		vec3 currentPoint = points[i];
		printf("(Point)");
		printVector(currentPoint);
	}
	for(int i = 0; i < totalNumVertices; i++) {
		vec3 currentNormal = normals[i];
		printf("(Normal)");
		printVector(currentNormal);
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
		printPointsAndNormals();
	}

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(normals) + sizeof(lineBufferData), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );

    setLineBufferData();
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals),
        sizeof(lineBufferData), lineBufferData );


    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_TRUE, 0,
                           BUFFER_OFFSET(sizeof(normals)) );

    modelCentroid = calculateModelCentroid();

    modelMatrix = glGetUniformLocation(program, "modelMatrix");
    viewMatrix = glGetUniformLocation(program, "viewMatrix");
    projectionMatrix = glGetUniformLocation(program, "projectionMatrix");
    transformMatrix = glGetUniformLocation(program, "transformMatrix");
    l_ambient = glGetUniformLocation(program, "l_ambient");
	l_diffuse = glGetUniformLocation(program, "l_diffuse");
	l_specular = glGetUniformLocation(program, "l_specular");
	l_position = glGetUniformLocation(program, "l_position");
	m_reflect_ambient = glGetUniformLocation(program, "m_reflect_ambient");
	m_reflect_diffuse = glGetUniformLocation(program, "m_reflect_diffuse");
	m_reflect_specular = glGetUniformLocation(program, "m_reflect_specular");
	m_shininess = glGetUniformLocation(program, "m_shininess");
	eyePosition = glGetUniformLocation(program, "eyePosition");

	glClearColor( 1.0, 1.0, 1.0, 1.0 ); // grey background



    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    printf("Num Vertices Used: %d\n",totalNumVertices);


    glPointSize(20.0f);
    glLineWidth(20.0f);
    glDrawArrays( GL_TRIANGLES, 0, totalNumVertices );
    glDrawArrays( GL_POINTS, 0, totalNumVertices );
    glDrawArrays( GL_LINES, 2* defaultSize, 6 );

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

   calculateEyeVector();

   // Camera matrix
   mat4 View = LookAt(
	EyeVector,
	vec4(0,0,0,1),
	vec4(0,1,0,0)

       );

   // Move model to the origin mat4(1.0f);
   mat4 Model = Translate(-1 * modelCentroid);

   TransformMatrix = Projection * View * Model;

   // Following uni marburg example, do I need to invert my matrix
   //
   mat4 ModelViewMatrix = View * Model;

   glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, Model );
   glUniformMatrix4fv( viewMatrix, 1, GL_TRUE, View );
   glUniformMatrix4fv( projectionMatrix, 1, GL_TRUE, Projection );

   glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );
   glUniform3fv(l_ambient, 1, L_ambient);
   glUniform3fv(l_diffuse, 1, L_diffuse);
   glUniform3fv(l_specular, 1, L_specular);
   glUniform3fv(l_position, 1, L_position);


   glUniform3fv(m_reflect_ambient, 1, M_reflect_ambient);
   glUniform3fv(m_reflect_diffuse, 1, M_reflect_diffuse);
   glUniform3fv(m_reflect_specular, 1, M_reflect_specular);
   glUniform1f(m_shininess,M_shininess);
   glUniform3fv(m_reflect_specular, 1, M_reflect_specular);
   glUniform3fv(eyePosition,1,EyeVector);
   glUniform1f(m_shininess,M_shininess);


   glClearColor( 1.0, 1.0, 1.0, 1.0 ); // grey background


   glDrawArrays( GL_TRIANGLES, 0, totalNumVertices );

   /*
   glDrawArrays( GL_POINTS, 0, totalNumVertices );
   glDrawArrays( GL_LINES, 2*defaultSize, 6 ); */

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


vec3 calculateFaceNormal(vec3 vertex1, vec3 vertex2, vec3 vertex3) {
		// See p 272
		vec3 U = vertex2 - vertex1;
		vec3 V = vertex3 - vertex2;

		vec3 crossVector = cross(U,V);

		double customLength = sqrt(crossVector.x*crossVector.x+crossVector.y*crossVector.y+crossVector.z*crossVector.z);

		vec3 customNormal = crossVector / customLength;

		vec3 absCustomNormal = vAbs(customNormal);

		if(std::isnan(absCustomNormal.x) || std::isnan(absCustomNormal.y) || std::isnan(absCustomNormal.z)) {

			printf("Invalid cross vector");

			exit(1);
		}
		return absCustomNormal;
}

// Tesselate the points
void tesselateAndCalculateNormals() {

	int numTesselatedVertices = 0;

	for(int i = 0; i < uRange - 1; i++) {

		for(int j=0; j < vRange - 1; j++) {

			printf("Tesselate - i is: %d, j is: %d\n",i,j);

			// First triangle
			vec3 vertex1 =  interpolatedPoints[i][j]; // 1
			vec3 vertex2 = interpolatedPoints[i+1][j]; // 2
			vec3 vertex3 = interpolatedPoints[i][j+1]; // 3

			printf("Calling calulcateFaceNormal for:");
			printVector(vertex1);
			printVector(vertex2);
			printVector(vertex3);

			vec3 normal1 = calculateFaceNormal(vertex1,vertex2,vertex3);

			points[numTesselatedVertices] = vertex1;
			normals[numTesselatedVertices] = normal1;
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex2;
			normals[numTesselatedVertices] = normal1;

			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex3;
			normals[numTesselatedVertices] = normal1;
			numTesselatedVertices++;

			// Second triangle
			vec3 vertex4 = interpolatedPoints[i+1][j]; // 4 == 2, not 1
			vec3 vertex5 = interpolatedPoints[i+1][j+1]; // 5
			vec3 vertex6 = interpolatedPoints[i][j+1]; // 6

			printf("Calling calulcateFaceNormal for:");
			printVector(vertex4);
			printVector(vertex5);
			printVector(vertex6);


			vec3 normal2 = calculateFaceNormal(vertex4,vertex5,vertex6);

			points[numTesselatedVertices] = vertex4;
			normals[numTesselatedVertices] = normal2;
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex5;
			normals[numTesselatedVertices] = normal2;
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex6;
			normals[numTesselatedVertices] = normal2;
			numTesselatedVertices++;
		}
	}

	totalNumVertices = numTesselatedVertices;
}

void parseControlVerticesToPatch() {
	// Convert the 16 control vertices into a 4 by 4 array

	patch[0][0] = vec3(0.0f,0.0f,0.0f);
	patch[1][0] = vec3(2.0f,0.0f,1.5f);
	patch[2][0] = vec3(4.0f,0.0f,2.9f);
	patch[3][0] = vec3(6.0f,0.0f,0.0f);

	patch[0][1] = vec3(0.0f,2.0f,1.1f);
	patch[1][1] = vec3(2.0f,2.0f,3.9f);
	patch[2][1] = vec3(4.0f,2.0f,3.1f);
	patch[3][1] = vec3(6.0f,2.0f,0.7f);

	patch[0][2] = vec3(0.0f,4.0f,-0.5f);
	patch[1][2] = vec3(2.0f,4.0f,2.6f);
	patch[2][2] = vec3(4.0f,4.0f,2.4f);
	patch[3][2] = vec3(6.0f,4.0f,0.4f);

	patch[0][3] = vec3(0.0f,6.0f,0.3f);
	patch[1][3] = vec3(2.0f,6.0f,-1.1f);
	patch[2][3] = vec3(4.0f,6.0f,1.3f);
	patch[3][3] = vec3(6.0f,6.0f,-0.2f);

}

void reinitializeArrays() {
	// Reset all arrays
	for(int i = 0; i < 10000; i++) {
		points[i] = vec3(0,0,0);
		normals[i] = vec3(0,0,0);
	}
}

void printUsage() {
	std::cout << "Press: 1 - To increase camera height" << std::endl;
	std::cout << "Press: 2 - To decrease camera height" << std::endl;
	std::cout << "Press: 3 - To increase orbit radius" << std::endl;
	std::cout << "Press: 4 - To decrease orbit radius" << std::endl;
	std::cout << "Press: 5 - To increase camera angle (rotate camera)" << std::endl;
	std::cout << "Press: 6 - To decrease camera angle (rotate counterclockwise)" << std::endl;
	std::cout << "Press: j - To increase the sampling by 1 (e.g., from 10x10 default to 11x11)" << std::endl;
	std::cout << "Press: k - To decrease the sampling by 5 (e.g., from 10x10 default to 9x9)" << std::endl;
	std::cout << "Press n to cycle thru control points and then -,=,[,],{,} to increase/decrease x,y,z respectively" << std::endl;
	std::cout << "Press: z - To reset the view" << std::endl;
	std::cout << "Press: x - To exit the program" << std::endl;

}


void drawWindowAtSelectedSample(int uRange, int vRange) {

	reinitializeArrays();

	// Interpolate as desired

	interpolatePatch(uRange, vRange);

	tesselateAndCalculateNormals();

    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );

    modelCentroid = calculateModelCentroid();

	glutPostRedisplay();
}

void setDefaultViewParams() {
	L_ambient = vec3(1.0, 1.0, 1.0);
	L_diffuse = vec3(1.0, 1.0, 1.0);
	L_specular = vec3(1.0, .5, .5);
	M_reflect_ambient = vec3(0.7, .3, .7);
	M_reflect_diffuse = vec3(0.2, .6, .2);
	M_reflect_specular = vec3(0.1, .1, .1);
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
	calculateModelCentroid();
	uRange = 10;
	vRange = 10;
}

void
keyboard( unsigned char key, int x, int y )
{

    switch ( key ) {

    case '1' :
    	// Increase Height
    	Height += HeightDelta;
    	break;
    case '2' :
    	// Decrease height
    	Height -= HeightDelta;
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
    case '5' :
    	// Rotate counterclockwise
    	Theta += 5;
    	Theta = Theta % 360;
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
    	}

    	break;
    case '6' :
    	Theta -= 5;
    	Theta = Theta % 360;
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
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
    case 'z':
    	if(debug) {
    		printf("Reset all values\n");
    	}

		setDefaultViewParams();
    	break;

    case 'j':

    	uRange += 1;
    	vRange += 1;

    	drawWindowAtSelectedSample(uRange, vRange);

    	break;

    case 'k' :

    	uRange -= 1;
    	vRange -= 1;

    	drawWindowAtSelectedSample(uRange, vRange);
    	break;

    case 'n' :
    	selectedPointIdx++;
    	selectedPointIdx = selectedPointIdx % 16;
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	printUsage();
    	break;

    case '-' :
    	printf("Increase control point x axis\n");
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	controlVertices[selectedPointIdx].x += 1;
    	printf("Modified control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	drawWindowAtSelectedSample(uRange,vRange);
    	printf("Point movement done\n");
    	printUsage();

    	break;
    case '=' :
    	printf("Decrease control point x axis\n");
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	controlVertices[selectedPointIdx].x -= 1;
    	printf("Modified control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	drawWindowAtSelectedSample(uRange,vRange);
    	printf("Point movement done\n");
    	printUsage();
    	break;
    case '[' :
    	printf("Increase control point y axis\n");
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	controlVertices[selectedPointIdx].y += 1;
    	printf("Modified control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	drawWindowAtSelectedSample(uRange,vRange);
    	printf("Resampling done\n");
    	printUsage();
    	break;
    case ']' :
    	printf("Decrease control point y axis\n");
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	controlVertices[selectedPointIdx].y -= 1;
    	printf("Modified control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	drawWindowAtSelectedSample(uRange,vRange);
    	printf("Resampling done\n");
    	printUsage();
    	break;
    case '{' :
    	printf("Increase control point z axis\n");
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	controlVertices[selectedPointIdx].z += 1;
    	printf("Modified control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	drawWindowAtSelectedSample(uRange,vRange);
    	printf("Resampling done\n");
    	printUsage();
    	break;
    case '}' :
    	printf("Decrease control point z axis\n");
    	printf("Selected control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	controlVertices[selectedPointIdx].z -= 1;
    	printf("Modified control point %d, x: %f, y %f, z: %f\n",selectedPointIdx,controlVertices[selectedPointIdx].x,controlVertices[selectedPointIdx].y,controlVertices[selectedPointIdx].z);
    	drawWindowAtSelectedSample(uRange,vRange);
    	printf("Resampling done\n");
    	printUsage();
    	break;

	case 'x':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    }

	calculateEyeVector();
	glutPostRedisplay();

}

int
main( int argc, char **argv )
{
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


    setDefaultViewParams();

	// Convert the 16 control vertices into a 4 by 4 array
	parseControlVerticesToPatch();

	drawWindowAtSelectedSample(uRange,vRange);

    initMainWindow();

    printUsage();

	glutDisplayFunc( displayMainWindow );
	glutKeyboardFunc( keyboard );

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}
