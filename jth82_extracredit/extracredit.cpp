// Make a 3D cube

#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

bool debug = false;

GLuint modelViewMatrix, projectionMatrix, currentTransformMatrix;

float Joint1Angle = 45.0f;

// Uniforms for lighting
// Light properties

vec3 P_ambient, P_diffuse, P_specular;

float M_shininess = 100;

GLuint p_ambient, p_diffuse, p_specular, l_position, e_position, m_shininess;

float eps = 0.001;

bool isPerspective = false;

GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -4.0, top = 4.0;
GLfloat  near = -10.0, far = 10.0;

const int NumVertices = 10000; //(6 faces)(2 triangles/face)(3 vertices/triangle)

// Total number of vertices
int shape1VertexCount;

vec3 points[NumVertices];
vec3 normals[NumVertices];

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

vec3 vecRadians(vec3 degrees) {
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
	// Base color is red
	P_ambient = vProduct(vec3(0.15, .15, .15),vec3(1,1,1));
	P_diffuse = vProduct(vec3(0.6, .6, .6),vec3(.5,0,0));
	P_specular = vProduct(vec3(0.25, .25, .25),vec3(1,1,1));
	M_shininess = 50;
	Radius = 8.0;
	Height = 3.0f;
	Theta = 85.0f;
	LightTheta = Theta;
	LightRadius = Radius;
	LightHeight = Height;
	RadiusDelta = 1;
	isPerspective = true;
	Joint1Angle = 5.0f;
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

// Vertices of a unit cube centered at origin, sides aligned with axes
/*
vec3 unitCubeVertices[8] = {
		vec3( 0, 0,  1),
		vec3( 0,  1,  1 ),
		vec3(  1,  1,  1),
		vec3(  1, 0,  1 ),
		vec3( 0, 0, 0 ),
		vec3( 0,  1, 0 ),
		vec3(  1,  1, 0 ),
		vec3(  1, 0, 0 )
};
*/

vec3 baseVertices[8] = {
    vec3( -0.5, -0.1,  0.5),
	vec3( -0.5,  0.1,  0.5),
	vec3(  0.5,  0.1,  0.5),
	vec3(  0.5, -0.1,  0.5),
	vec3( -0.5, -0.1, -0.5),
	vec3( -0.5,  0.1, -0.5),
	vec3(  0.5,  0.1, -0.5),
	vec3(  0.5, -0.1, -0.5)
};

vec3 bodyVertices[8] = {
    vec3( -0.1, -0.1,  0.2),
	vec3( -0.1,  4,  0.2),
	vec3(  0.1,  4,  0.2),
	vec3(  0.1, -0.1,  0.2),
	vec3( -0.1, -0.1, -0.2),
	vec3( -0.1,  4, -0.2),
	vec3(  0.1,  4, -0.2),
	vec3(  0.1, -0.1, -0.2)
};

vec3 arm1Vertices[8] = {
    vec3( -0.1, -0.1,  0.2),
	vec3( -0.1,  .1,  0.2),
	vec3(  4,  .1, 0.2),
	vec3(  4, -0.1,  0.2),
	vec3( -0.1, -0.1, -0.2),
	vec3( -0.1,  .1, -0.2),
	vec3(  4,  .1, -0.2),
	vec3(  4, -0.1, -0.2)
};

vec3 unitCubeVertices[8] = {
    vec3( -0.5, -0.5,  0.5),
	vec3( -0.5,  0.5,  0.5),
	vec3(  0.5,  0.5,  0.5),
	vec3(  0.5, -0.5,  0.5),
	vec3( -0.5, -0.5, -0.5),
	vec3( -0.5,  0.5, -0.5),
	vec3(  0.5,  0.5, -0.5),
	vec3(  0.5, -0.5, -0.5)
};

// quad generates two triangles for each face and assigns colors
//    to the vertices. Draws a, b, c as one triangle, a, c, d as another
int Index = 0;
void
quad( int a, int b, int c, int d, vec3 vertexSet[] )
{
	vec3 point1 = vertexSet[a];
	vec3 point2 = vertexSet[b];
	vec3 point3 = vertexSet[c];

	vec3 U = point2 - point1;
	vec3 V = point3 - point2;
	vec3 face1Normal = vec3(1.0,1.0,1.0); // normalize(cross(U,V));

	vec3 point4 = vertexSet[d];

    points[Index] = point1; normals[Index] = face1Normal; Index++;
    points[Index] = point2; normals[Index] = face1Normal; Index++;
    points[Index] = point3; normals[Index] = face1Normal; Index++;

    U = point3 - point1;
    V = point4 - point3;
    vec3 face2Normal = vec3(1.0,1.0,1.0); // normalize(cross(U,V));

    points[Index] = point1; normals[Index] = face2Normal; Index++;
    points[Index] = point3; normals[Index] = face2Normal; Index++;
    points[Index] = point4; normals[Index] = face2Normal; Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors

void draw(vec3 refVecs[]) {
    quad( 1, 0, 3, 2, refVecs );
    quad( 2, 3, 7, 6, refVecs);
    quad( 3, 0, 4, 7, refVecs );
    quad( 6, 5, 1, 2, refVecs );
    quad( 4, 5, 6, 7, refVecs );
    quad( 5, 4, 0, 1, refVecs );
}

mat4 getTransformMatrix(vec3 scaleVec, vec3 rotationVec, vec3 translationVec) {
    mat4 scaleMatrix;
    scaleMatrix[0][0] = scaleVec.x;
    scaleMatrix[1][1] = scaleVec.y;
    scaleMatrix[2][2] = scaleVec.z;


    vec3 angles = vecRadians(-1.0f * rotationVec);
    printVector(angles);

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

    // Translate
    mat4 translationMatrix;
    translationMatrix[0][3] = translationVec.x;
    translationMatrix[1][3] = translationVec.y;
    translationMatrix[2][3] = translationVec.z;

    mat4 currentMatrix = scaleMatrix * rotationMatrix * translationMatrix;
    return currentMatrix;

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

    currentTransformMatrix = glGetUniformLocation(program, "CurrentTransformMatrix");
    projectionMatrix = glGetUniformLocation(program, "Projection");
    modelViewMatrix = glGetUniformLocation(program, "ModelView");
    p_ambient = glGetUniformLocation(program, "AmbientProduct");
	p_diffuse = glGetUniformLocation(program, "DiffuseProduct");
	p_specular = glGetUniformLocation(program, "SpecularProduct");
	l_position = glGetUniformLocation(program, "LightPosition");
	e_position = glGetUniformLocation(program, "EyePosition");
	m_shininess = glGetUniformLocation(program, "Shininess");

    glClearColor( .50, .50, .50, 1.0 ); // black background

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glFlush();

}



void
displayMainWindow( void )
{

   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   mat4 Projection;

   if(isPerspective) {
	   Projection = Perspective(90.0f, 1.0f, 0.1f, 100.0f);
   }
   else {
	   Projection = Ortho(left,right,bottom,top,-0.001f,100.f);
   }


   vec3 eyePos = calculateEyeVector();
   vec4 lightPos = calculateLightVector();

   // Look at model centroid
   mat4 model_view = LookAt(
	eyePos,
	vec4(0,0,0,0),
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

   printf("Theta is: %f\n,",Theta);

   mat4 baseTranslate = Translate(vec3(0,-5,0));
   mat4 baseRotate = RotateX(0.0f);
   mat4 baseScale = Scale(vec3(1,.4,1));
   mat4 baseCTM = baseScale *baseRotate * baseTranslate;

   baseCTM = mat4(1);

   glUniformMatrix4fv(currentTransformMatrix, 1, GL_TRUE, baseCTM);
   glDrawArrays(GL_TRIANGLES,0,36);


   mat4 body1Translate = Translate(vec3(0,0,0));
   mat4 body1Rotate = RotateX(0.0f);
   mat4 body1Scale = Scale(vec3(.2,4,1));

   mat4 body1CTM = body1Scale *body1Rotate * body1Translate;
   body1CTM = mat4(1.0);
   glUniformMatrix4fv(currentTransformMatrix, 1, GL_TRUE, body1CTM);
   glDrawArrays(GL_TRIANGLES,36,36);


   mat4 arm1Translate = Translate(vec3(0,4,0));
   mat4 arm1Rotate = RotateY(Joint1Angle);

   mat4 arm1CTM = arm1Translate * arm1Rotate;
   glUniformMatrix4fv(currentTransformMatrix, 1, GL_TRUE, arm1CTM);
   glDrawArrays(GL_TRIANGLES,72,36);

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
    	isPerspective = true;
    	break;
    case '8' :
    	isPerspective = false;
    	break;
	case 'x':
		// Exit
		exit( EXIT_SUCCESS );
		break;
	case 't':
		Joint1Angle += 5;
		printf("Incrementing Joint 1 angle to: %f\n");

		break;
	case 'r':
		Joint1Angle -= 5;
		printf("Decrementing Joint 1 angle to: %f\n");

		break;
    case 'z':
    	setDefaultViewParams();
    	break;

    }


	glutPostRedisplay();

}

vec3 calculateNormal(vec3 vertex1, vec3 vertex2, vec3 vertex3) {
	vec3 U = vertex2 - vertex1;
	vec3 V = vertex3 - vertex2;
	return normalize(cross(U,V));
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

    glutCreateWindow( "Extra Credit" );
#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif


	setDefaultViewParams();

	// The base
	draw(baseVertices);

	// First pillar
	draw(bodyVertices);

	// First arm which connects at first joint
	draw(arm1Vertices);


	std::cout << "Press: 1 - To increase camera height" << std::endl;
	std::cout << "Press: 2 - To decrease camera height" << std::endl;
	std::cout << "Press: 3 - To increase orbit radius" << std::endl;
	std::cout << "Press: 4 - To decrease orbit radius" << std::endl;
	std::cout << "Press: 5 - To increase camera angle (rotate camera)" << std::endl;
	std::cout << "Press: 6 - To decrease camera angle (rotate counterclockwise)" << std::endl;
	std::cout << "Press: 7 - To enable perspective projection mode" << std::endl;
	std::cout << "Press: 8 - To enable parallel projection mode (default)" << std::endl;
	std::cout << "Press: z - To reset the view" << std::endl;
	std::cout << "Press: x - To exit the program" << std::endl;


	initMainWindow();

	glutDisplayFunc( displayMainWindow );
	glutKeyboardFunc( keyboard );

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}
