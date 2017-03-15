#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>

bool debug = false;


GLuint textures[1];
GLuint vTexCoord;
GLuint texture;


vec3 P_ambient, P_diffuse, P_specular;

float M_shininess = 100;

GLuint modelViewMatrix, projectionMatrix;

GLuint p_ambient, p_diffuse, p_specular, l_position, e_position, m_shininess;

float eps = 0.001;

bool isPerspective = true;

GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;

// Is set in tesellateInterpolatedPoints, and used in initMainWindow
int totalNumVertices;

const int defaultSize = 100000;
vec3 points[defaultSize];
vec3 normals[defaultSize];
vec2 tex_coords[defaultSize];
vec3 controlVertices[defaultSize];
vec3 patch[4][4];

int uRange = 12;
int vRange = 12;

const int maxRange = 51;
vec3 interpolatedPoints[maxRange][maxRange];

vec3 lineBufferData[6];

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
int border = 50;


float pixels[] = {
    0.0f, 0.0f, 0.0f,   .25f, .25f, .25f,
    .50f, .5f, .5f,   1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,   .5f, .5f, .5f,
    .25f, .25f, .25f,   0.0f, 0.0f, 0.0f,
};

vec3 vProduct(vec3 a, vec3 b) {
	return vec3(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
}

void initTexture() {
	// procedurally generate 2D RGB image data
}

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

vec3 calculateModelCentroid() {
	vec3 sumOfAllPoints;
	for(int i = 0; i < totalNumVertices; i++) {
		sumOfAllPoints += points[i];
	}
	vec3 centroid = (sumOfAllPoints) / totalNumVertices;
	return centroid;
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
       sizeof(normals) + sizeof(tex_coords), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0,
        sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points),
        sizeof(normals), normals );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals),
        sizeof(tex_coords), tex_coords );



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
                           BUFFER_OFFSET(sizeof(points)) );

    GLuint vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(points) + sizeof(normals)) );


    modelCentroid = calculateModelCentroid();

    projectionMatrix = glGetUniformLocation(program, "Projection");
    modelViewMatrix = glGetUniformLocation(program, "ModelView");
    p_ambient = glGetUniformLocation(program, "AmbientProduct");
	p_diffuse = glGetUniformLocation(program, "DiffuseProduct");
	p_specular = glGetUniformLocation(program, "SpecularProduct");
	l_position = glGetUniformLocation(program, "LightPosition");
	e_position = glGetUniformLocation(program, "EyePosition");
	m_shininess = glGetUniformLocation(program, "Shininess");
	texture = glGetUniformLocation(program, "texture");

	   glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background

	// Texture objects
    glGenTextures( 1, textures );
    glBindTexture( GL_TEXTURE_2D, textures[0] );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 4,
       4, 0, GL_RGB, GL_FLOAT, pixels );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
         GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D,
          GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D,
          GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glActiveTexture( GL_TEXTURE0 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    printf("Num Vertices Used: %d\n",totalNumVertices);

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
   glUniform1i( texture, 0 );


   glClearColor( 0.0, 0.0, 0.0, 0.0 ); // black background


   glDrawArrays( GL_TRIANGLES, 0, totalNumVertices );

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

vec3 calculateNormal(vec3 vertex1, vec3 vertex2, vec3 vertex3) {
	vec3 U = vertex2 - vertex1;
	vec3 V = vertex3 - vertex2;
	return normalize(cross(U,V));
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

			vec3 normal1 = calculateNormal(vertex1,vertex2,vertex3);

			points[numTesselatedVertices] = vertex1;
			normals[numTesselatedVertices] = normal1;
			tex_coords[numTesselatedVertices] = vec2( 0.0, 0.0 );
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex2;
			normals[numTesselatedVertices] = normal1;
			tex_coords[numTesselatedVertices] = vec2( 0.0, 1.0 );
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex3;
			normals[numTesselatedVertices] = normal1;
			tex_coords[numTesselatedVertices] = vec2( 0.0, 0.0 );
			numTesselatedVertices++;

			// Second triangle
			vec3 vertex4 = interpolatedPoints[i+1][j]; // 4 == 2, not 1
			vec3 vertex5 = interpolatedPoints[i+1][j+1]; // 5
			vec3 vertex6 = interpolatedPoints[i][j+1]; // 6

			printf("Calling calulcateFaceNormal for:");
			printVector(vertex4);
			printVector(vertex5);
			printVector(vertex6);


			vec3 normal2 = calculateNormal(vertex4,vertex5,vertex6);

			points[numTesselatedVertices] = vertex4;
			normals[numTesselatedVertices] = normal2;
			tex_coords[numTesselatedVertices] = vec2( 0.0, 0.0 );
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex5;
			normals[numTesselatedVertices] = normal2;
			tex_coords[numTesselatedVertices] = vec2( 0.0, 1.0 );
			numTesselatedVertices++;

			points[numTesselatedVertices] = vertex6;
			normals[numTesselatedVertices] = normal2;
			tex_coords[numTesselatedVertices] = vec2( 0.0, 0.0 );
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
	// Base color is red
	P_ambient = vProduct(vec3(0.15, .15, .15),vec3(1,1,1));
	P_diffuse = vProduct(vec3(0.6, .6, .6),vec3(.5,0,0));
	P_specular = vProduct(vec3(0.25, .25, .25),vec3(1,1,1));
	M_shininess = 100;
	Radius = 12.0;
	Height = 0.0f;
	Theta = 270.0f;
	LightTheta = Theta;
	LightRadius = Radius;
	LightHeight = Height;
	RadiusDelta = 1;
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

    mainWindow = glutCreateWindow( "Assignment 9 - Part 1" );
#ifndef __APPLE__
    GLenum err = glewInit();

    if (GLEW_OK != err)
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
#endif


    setDefaultViewParams();
    initTexture();

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
