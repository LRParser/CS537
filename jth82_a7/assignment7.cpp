// Make a 3D cube

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

bool debug = true;

mat4 TransformMatrix;
GLuint transformMatrix;

// Uniforms for lighting
// Light properties

point4 L_position = point4(0,5,5,1);

// Material properties



vec4 materialAmbientLightProperties[3];
vec4 materialDiffuseLightProperties[3];
vec4 materialSpecularLightProperties[3];

vec4 materialAmbientReflectionProperties[3];
vec4 materialDiffuseReflectionProperties[3];
vec4 materialSpecularReflectionProperties[3];


color4 L_ambient = vec4(0.0,1,0.0,1);
color4 L_diffuse = vec4(0.0,1,0.0,1);
color4 L_specular = vec4(1.0,1.0,1.0,1);

color4 M_reflect_ambient = vec4(0.0,1,0.0,1.0);
color4 M_reflect_diffuse = vec4(0.0,1,0.0,1.0);
color4 M_reflect_specular = vec4(1.0,1,1.0,1.0);

float M_shininess = 10;

GLuint l_ambient, l_diffuse, l_specular, l_position, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, m_shininess;
GLuint cameraPosition;
GLuint isGouraud;

// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
bool isPerspective = true;

// For Ortho coordinates
GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;

float IsGouraud = .6; // >.5 is true, otherwise false


// Copied from Lecture 8 slides as described in assignment

const int NumVertices = 10000; //(6 faces)(2 triangles/face)(3 vertices/triangle)
int NumVerticesUsed = 0;

vec4 smfVertices[NumVertices];

vec4 points[100];
vec4 normals[100];

vec4 EyeVector = vec4(1.0f,1.0f,10.0f,1.0f);

vec4 modelCentroid;

float Radius = 10.0;
int Theta = 90; // Longitude angle in degrees
int LightTheta = -345;
int LightRadius = -1;
float Height, LightHeight = 1;

float RadiusDelta = 1;
int Delta = 5;
float HeightDelta = .1;
float ParallelDelta = 2;

int mainWindow;

int w = 500;
int h = 500;
int border = 50;

vec2 tVertices[6] = {
    vec2(-.75,-.75), vec2(-.75,0),vec2(0,-.75),
    vec2(-.75,0 ), vec2(0,-.75), vec2( 0, 0 )
};


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

float patch[4][4][3] = {
		{
			{ 0.0, 0.0, 0.0 },
			{ 2.0, 0.0, 1.5},
			{ 4.0, 0.0, 2.9 },
			{ 6.0, 0.0, 0.0 }
		},
		{
			{ 0.0, 2.0, 1.1 },
			{ 2.0, 2.0, 3.9 },
			{ 4.0, 2.0, 3.1 },
			{ 6.0, 2.0, 0.7 }
		},
		{
			{ 0.0, 4.0, -0.5},
			{ 2.0, 4.0, 2.6 },
			{ 4.0, 4.0, 2.4 },
			{ 6.0, 4.0, 0.4 }
		},
		{
			{ 0.0, 6.0, 0.3 },
			{ 2.0, 6.0, -1.1},
			{ 4.0, 6.0, 1.3 },
			{ 6.0, 6.0, -0.2}
		}
};


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

vec4 calcPatchPoints() {

	for(int u = 0; u < 10; u++) {

		float uParam = (float) u / 10.0f;

		for(int v = 0; v < 10; v++) {

			float vParam = (float) v / 10.0f;

			vec4 pointSum = vec4(0,0,0,0);

			for(int i = 0; i < 4; i++) {

				float bernsteinForU = getBernsteinFactor(uParam,i);

				for(int j = 0; j < 4; j++) {

					float bernsteinForJ = getBernsteinFactor(vParam,j);

					float controlX = patch[i][j][0];
					float controlY = patch[i][j][1];
					float controlZ = patch[i][j][2];
					vec4 controlPoint = vec4(controlX,controlY,controlZ,1);
					float weight = bernsteinForU * bernsteinForJ;
					pointSum += weight * controlPoint;

				}
			}


			points[NumVerticesUsed] = pointSum;
			normals[NumVerticesUsed] = vec4(.3,.3,.3,0);
			NumVerticesUsed++;
			printf("v %f %f %f\n",pointSum.x,pointSum.y,pointSum.z);


		}

	}
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

void initOld() {
	// Specify the vertices for a triangle
	    vec2 vertices[6] = {
	        vec2(-.75,-.75), vec2(-.75,0),vec2(0,-.75),
	        vec2(-.75,0 ), vec2(0,-.75), vec2( 0, 0 )
	    };

	    // Create a vertex array object
	    GLuint vao[1];
	    glGenVertexArrays( 1, vao );
	    glBindVertexArray( vao[0] );


	    // Create and initialize a buffer object
	    GLuint buffer;
	    glGenBuffers( 1, &buffer );
	    glBindBuffer( GL_ARRAY_BUFFER, buffer );
	    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

	    // Load shaders and use the resulting shader program
	    GLuint program = InitShader( "vshader101.glsl", "fshader101.glsl" );
	    //  glUseProgram( program );  // This is called in InitShader

	    // Initialize the vertex position attribute from the vertex shader
	    GLuint loc = glGetAttribLocation( program, "vPosition" );
	    glEnableVertexAttribArray( loc );
	    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
	                           BUFFER_OFFSET(0) );

	    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

}



void
initMainWindow( void )
{


    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );


    calcPatchPoints();

    // Print points and normals info
	if(debug) {
		for(int i = 0; i < NumVerticesUsed; i++) {
			vec4 currentPoint = points[i];
			printf("%f %f %f\n",currentPoint.x,currentPoint.y,currentPoint.z);
			//printVector(currentPoint);
		}

	}

	/*
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
*/
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );

    /*
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) +
       sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );
*/
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader101.glsl", "fshader101.glsl" );

    // Initialize the vertex position attribute from the vertex shader
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

/*
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_TRUE, 0,
                           BUFFER_OFFSET(sizeof(normals)) );


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


    modelCentroid = calculateModelCentroid();
*/

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
    glPointSize(10.0f);

    //glDrawArrays( GL_LINE_STRIP, 0, 100 );
    //glFlush();

}

void displayOld() {
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_TRIANGLES, 0, 100 );    // draw the points
    glFlush();
}

void
displayMainWindow( void )
{
	   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	   glDrawArrays(GL_TRIANGLES, 0, 100);


	   /*
    glBegin(GL_POINTS); //starts drawing of points
    for(int i = 0; i < 100; i++) {
      glVertex3f(points[i].x,points[i].y,0.0f);//upper-right corner
      //glVertex3f(-1.0f,-1.0f,0.0f);//lower-left corner
    }
    glEnd();//end drawing of points
*/
	/*
	   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glDrawArrays( GL_LINE_STRIP, 0, 100 );

	   glutSwapBuffers();
*/
	/*
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

   /*
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

   glDrawArrays( GL_TRIANGLES, 0, NumVerticesUsed );

   */
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
    case 'a' :
    	if(debug) {
    		printf("Material 1 selected");
    	}
    	L_ambient = vec4(1.0,1,0.0,1);
    	L_diffuse = vec4(0.0,1,0.0,1);
    	L_specular = vec4(1.0,1.0,1.0,1);

    	M_reflect_ambient = vec4(0.0,1,0.0,1.0);
    	M_reflect_diffuse = vec4(0.0,1,0.0,1.0);
    	M_reflect_specular = vec4(1.0,1,1.0,1.0);

    	break;

    case 's' :
    	if(debug) {
    		printf("Material 2 selected");
    	}
    	L_ambient = vec4(1.0,1,1,1);
    	L_diffuse = vec4(0.0,0,1.0,1);
    	L_specular = vec4(0.0,1.0,1.0,1);

    	M_reflect_ambient = vec4(1.0,0,1.0,1.0);
    	M_reflect_diffuse = vec4(1.0,.4,0.0,1.0);
    	M_reflect_specular = vec4(0.0,.4,.4,1.0);
    	pressed = true;

    	break;

    case 'd' :
    	if(debug) {
    		printf("Material 3 selected");
    	}
    	L_ambient = vec4(1.0,1,1,1);
    	L_diffuse = vec4(0.0,0,1.0,1);
    	L_specular = vec4(0.0,1.0,1.0,1);

    	M_reflect_ambient = vec4(0.0,1,0.0,1.0);
    	M_reflect_diffuse = vec4(1.0,1,0.0,1.0);
    	M_reflect_specular = vec4(1.0,1,.5,1.0);
    	pressed = true;

    	break;

	case 'x':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    }

	// calculateEyeVector2();
	// glutPostRedisplay();

}


void idle() {


}



int
main( int argc, char **argv )
{
	//glEnable( GL_DEPTH_TEST );
    bool old = false;

    glutInit( &argc, argv );
#ifdef __APPLE__
    if(old) {
        glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA ); //
    }
    else {
    	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    }
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




    /*
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

	std::cout << "Press: x - To exit the program" << std::endl;
*/

	if(old) {
    initOld();

	glutDisplayFunc( displayOld );
	}
	else {
		initMainWindow();
		glutDisplayFunc( displayMainWindow );

	}
	//glutKeyboardFunc( keyboard );

	if(!old) {
		glEnable(GL_DEPTH_TEST);
	}

	glutMainLoop();
	return 0;
}
