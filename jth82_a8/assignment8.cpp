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

bool debug = false;


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

color4 M_reflect_ambient = vec4(0.2,.2,1,1.0);
color4 M_reflect_diffuse = vec4(0.3,1,.3,1.0);
color4 M_reflect_specular = vec4(.1,.1,.1,1.0);


float M_shininess = 1;

GLuint l_ambient, l_diffuse, l_specular, l_position, m_reflect_ambient, m_reflect_diffuse, m_reflect_specular, m_shininess;
GLuint cameraPosition;

// Projection matrix : 45° Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
bool isPerspective = true;

// For Ortho coordinates
GLfloat  left = -4.0, right = 4.0;
GLfloat  bottom = -3.0, top = 5.0;
GLfloat  near = -10.0, far = 10.0;


const int NumVertices = 10000; //(6 faces)(2 triangles/face)(3 vertices/triangle)

// End indices for shapes after calls to readSMF
int shape1VertexCount, shape2VertexCount, shape3VertexCount = 0;

vec4 smfVertices[NumVertices];
std::vector<Face> smfFaces;

vec4 points[10000];
vec4 normals[10000];

vec4 EyeVector = vec4(1.0f,1.0f,10.0f,1.0f);

vec4 modelCentroid;

float Radius = 15.0;
int Theta = 90; // Longitude angle in degrees
int LightTheta = Theta + 10;
int LightRadius = -1;
float Height, LightHeight = 1;

float RadiusDelta = 1;
int Delta = 5;
float HeightDelta = .1;
float ParallelDelta = 2;

int mainWindow;

int windowWidth = 512;
int windowHeight = 512;

vec4 defaultColor = vec4(.5,0,0,0);
vec4 defaultBackgroundColor = vec4(.2,.2,.2,.2);

/* Frame buffer IDs */
GLuint fb, color_rb, depth_rb;
GLuint shade1Solid, shade2Solid,shade3Solid;
float Shade1Solid, Shade2Solid, Shade3Solid = 0.0;
vec4 framebufferColors[10000];

void initFrameBuffer() {
	   //RGBA8 RenderBuffer, 24 bit depth RenderBuffer, 512x512
	   glGenFramebuffers(1, &fb);
	   glBindFramebuffer(GL_FRAMEBUFFER, fb);

	   //Create and attach a color buffer
	   glGenRenderbuffers(1, &color_rb);

	   //We must bind color_rb before we call glRenderbufferStorage
	   glBindRenderbuffer(GL_RENDERBUFFER, color_rb);

	   //The storage format is RGBA8
	   glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, windowWidth, windowHeight);

	   //Attach color buffer to FBO
	   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	                                                 GL_RENDERBUFFER, color_rb);

	   //-------------------------
	   glGenRenderbuffers(1, &depth_rb);
	   glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);

	   //-------------------------
	   //Attach depth buffer to FBO
	   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							     GL_RENDERBUFFER, depth_rb);

	   glEnable(GL_DEPTH_TEST);


	   //-------------------------
	   //and now you can render to the FBO (also called RenderBuffer)
	   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
	   glClearColor(defaultBackgroundColor.x,defaultBackgroundColor.y,defaultBackgroundColor.z,defaultBackgroundColor.w);
	   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	   //-------------------------
	   glViewport(0, 0, windowWidth, windowHeight);

	   // Set note as to which buffer is being used

	   //-------------------------
	   // Render into frame buffer
	   //

	   // Set first color

	   glUniform1f(shade1Solid,0.6);
	   glUniform1f(shade2Solid,0.0);
	   glUniform1f(shade3Solid,0.0);

	   glDrawArrays( GL_TRIANGLES, 0, shape1VertexCount );

	   // Set second color

	   glUniform1f(shade1Solid,0.0);
	   glUniform1f(shade2Solid,0.6);
	   glUniform1f(shade3Solid,0.0);

	   glDrawArrays( GL_TRIANGLES, shape1VertexCount , shape2VertexCount );

	   // Set third color

	   glUniform1f(shade1Solid,0.0);
	   glUniform1f(shade2Solid,0.0);
	   glUniform1f(shade3Solid,0.6);

	   glDrawArrays( GL_TRIANGLES, shape1VertexCount + shape2VertexCount, shape3VertexCount);

	   glUniform1f(shade1Solid,0.0);
	   glUniform1f(shade2Solid,0.0);
	   glUniform1f(shade3Solid,0.0);

	   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}



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
	for(int i = 0; i < shape1VertexCount; i++) {
		if(debug) {
			printf("[Point]");
			printVector(points[i]);
		}
		sumOfAllPoints += points[i];
	}
	vec4 centroid = (sumOfAllPoints) / shape1VertexCount;
	if(debug) {
		printf("Model centroid");
		printVector(centroid);
	}
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
	EyeVector.w = 1;

	L_position.x = X;
	L_position.y = Y + .5;
	L_position.z = Z -2;
	L_position.w = 1;

	if(debug) {
		printf("Eye Vector\n");
		printVector(EyeVector);
	}


}

void setMaterialProperties() {
	// Set properties for each of 3 objects

	materialAmbientLightProperties[0] = vec4(1.0,1.0,1.0,1.0);
	materialDiffuseLightProperties[0] = vec4(1.0,1.0,1.0,0.5);
	materialSpecularLightProperties[0] = vec4(.5,.5,.5,1);

	materialAmbientReflectionProperties[0] = vec4(0.7,.6,.4,1.0);
	materialDiffuseReflectionProperties[0] = vec4(0.2,.3,.4,1.0);
	materialSpecularReflectionProperties[0] = vec4(.1,.1,.2,1.0);

	materialAmbientLightProperties[1] = vec4(1.0,1.0,1.0,1.0);
	materialDiffuseLightProperties[1] = vec4(1.0,1.0,1.0,.5);
	materialSpecularLightProperties[1] = vec4(0.5,.5,.5,1);

	materialAmbientReflectionProperties[1] = vec4(.2,.6,.4,1.0);
	materialDiffuseReflectionProperties[1] = vec4(.7,.3,.4,1.0);
	materialSpecularReflectionProperties[1] = vec4(.1,.1,.2,1.0);

	materialAmbientLightProperties[2] = vec4(1.0,1.0,1.0,1.0);
	materialDiffuseLightProperties[2] = vec4(1.0,1.0,1.0,.5);
	materialSpecularLightProperties[2] = vec4(0.5,.5,.5,1);

	materialAmbientReflectionProperties[2] = vec4(.1,.1,.2,1.0);
	materialDiffuseReflectionProperties[2] = vec4(.2,.3,.4,1.0);
	materialSpecularReflectionProperties[2] = vec4(.7,.6,.4,1.0);
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
		for(int i = 0; i < shape3VertexCount; i++) {
			vec4 currentPoint = points[i];
			printf("(Point)");
			printVector(currentPoint);
		}
		for(int i = 0; i < shape3VertexCount; i++) {
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
	shade1Solid = glGetUniformLocation(program, "shade1Solid");
	shade2Solid = glGetUniformLocation(program, "shade2Solid");
	shade3Solid = glGetUniformLocation(program, "shade3Solid");


	setMaterialProperties();

    glClearColor( defaultBackgroundColor.x,defaultBackgroundColor.y,defaultBackgroundColor.z,defaultBackgroundColor.w ); // black background

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

    glFlush();

    initFrameBuffer();

}

float r2() {
	return (float)((float) rand() / RAND_MAX);
}

void mouse(int button, int state, int x, int y) {
	if(state == GLUT_DOWN) {

		int buffer_x_loc = x;
		int buffer_y_loc = windowHeight - y;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
		GLubyte colorPicked[3];
		glReadPixels(buffer_x_loc,buffer_y_loc,1,1,GL_RGB,GL_UNSIGNED_BYTE,colorPicked);

		if(colorPicked[0] == 255) {
			materialAmbientReflectionProperties[0] =  vec4(r2(),r2(),r2(),1.0);

		}
		else if(colorPicked[1] == 255) {
			materialAmbientReflectionProperties[1] =  vec4(r2(),r2(),r2(),1.0);

		}
		else if(colorPicked[2] == 255) {
			materialAmbientReflectionProperties[2] =  vec4(r2(),r2(),r2(),1.0);

		}
		glutPostRedisplay();
	}

}

void
displayMainWindow( bool isFrameBuffer )
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

   glUniformMatrix4fv( transformMatrix, 1, GL_TRUE, TransformMatrix );

   glUniform4fv(cameraPosition,1,EyeVector);
   glUniform1f(m_shininess,M_shininess);
   glUniform4fv(l_position, 1, L_position);


   if(!isFrameBuffer) {
	   glUniform1f(shade1Solid,0.0);
	   glUniform1f(shade2Solid,0.0);
	   glUniform1f(shade3Solid,0.0);

	   glUniform4fv(l_ambient, 1, materialAmbientLightProperties[0]);
	   glUniform4fv(l_diffuse, 1, materialDiffuseLightProperties[0]);
	   glUniform4fv(l_specular, 1, materialSpecularLightProperties[0]);
	   glUniform4fv(m_reflect_ambient, 1, materialAmbientReflectionProperties[0]);
	   glUniform4fv(m_reflect_diffuse, 1, materialDiffuseReflectionProperties[0]);
	   glUniform4fv(m_reflect_specular, 1, materialSpecularReflectionProperties[0]);
	   glDrawArrays( GL_TRIANGLES, 0, shape1VertexCount );


	   glUniform4fv(l_ambient, 1, materialAmbientLightProperties[1]);
	   glUniform4fv(l_diffuse, 1, materialDiffuseLightProperties[1]);
	   glUniform4fv(l_specular, 1, materialSpecularLightProperties[1]);
	   glUniform4fv(m_reflect_ambient, 1, materialAmbientReflectionProperties[1]);
	   glUniform4fv(m_reflect_diffuse, 1, materialDiffuseReflectionProperties[1]);
	   glUniform4fv(m_reflect_specular, 1, materialSpecularReflectionProperties[1]);
	   glDrawArrays( GL_TRIANGLES, shape1VertexCount , shape2VertexCount );

	   glUniform4fv(l_ambient, 1, materialAmbientLightProperties[2]);
	   glUniform4fv(l_diffuse, 1, materialDiffuseLightProperties[2]);
	   glUniform4fv(l_specular, 1, materialSpecularLightProperties[2]);
	   glUniform4fv(m_reflect_ambient, 1, materialAmbientReflectionProperties[2]);
	   glUniform4fv(m_reflect_diffuse, 1, materialDiffuseReflectionProperties[2]);
	   glUniform4fv(m_reflect_specular, 1, materialSpecularReflectionProperties[2]);
	   glDrawArrays( GL_TRIANGLES, shape1VertexCount + shape2VertexCount, shape3VertexCount);

   }

   if(isFrameBuffer) {
	   glUniform1f(shade1Solid,0.6);
	   glUniform1f(shade2Solid,0.0);
	   glUniform1f(shade3Solid,0.0);
	   glDrawArrays( GL_TRIANGLES, 0, shape1VertexCount );
   }

   if(isFrameBuffer) {
	   glUniform1f(shade1Solid,0.0);
	   glUniform1f(shade2Solid,0.6);
	   glUniform1f(shade3Solid,0.0);
	   glDrawArrays( GL_TRIANGLES, shape1VertexCount , shape2VertexCount );
   }

   if(isFrameBuffer) {
	   glUniform1f(shade1Solid,0.0);
	   glUniform1f(shade2Solid,0.0);
	   glUniform1f(shade3Solid,0.6);
	   glDrawArrays( GL_TRIANGLES, shape1VertexCount + shape2VertexCount, shape3VertexCount);
   }



}

void display() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		displayMainWindow(false);
		glutSwapBuffers();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
		displayMainWindow(true);
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
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
    	}

    	break;
    case '6' :
    	Theta -= 5;
    	if(debug) {
    		printf("Theta is: %d\n",Theta);
    	}

    	break;


    	break;
    case '7' :
    	isPerspective = true;
    	break;
    case '8' :
    	isPerspective = false;
    	break;

    case 'z':
    	// Reset all values
    	if(debug) {
    		printf("Reset all values\n");
    	}

    	setMaterialProperties();

    	Radius = 15.0;
    	Theta = 90; // Longitude angle in degrees
    	Height = 1;

    	calculateModelCentroid();
    	break;

	case 'x':
		// Exit
		exit( EXIT_SUCCESS );
		break;

    }

	calculateEyeVector();
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


		if(debug) {
			printf("Normal for face %d incident to vertex %d is: ",it->faceIdx,vertexIdx);
			printVector(it->normal);

		}
		if(debug && std::isnan(it->normal.x)) {

			// We weren't able to calculate the normal. Set it to default color
			it->normal = normalize(defaultColor);
			printVector(incidentFacesColorsSum);
			printf("New sum is: ");
			printVector(incidentFacesColorsSum);
		}


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

int currentPointsIdx = 0;

void populatePointsAndNormalsArrays() {
	for(int i = 0; i < smfFaces.size(); i++) {
		Face currentFace = smfFaces.at(i);

		vec4 vertex1 = currentFace.firstVertex;

		vec4 vertex2 = currentFace.secondVertex;

		vec4 vertex3 = currentFace.thirdVertex;


		points[currentPointsIdx] = vertex1;
		points[currentPointsIdx + 1] = vertex2;
		points[currentPointsIdx + 2] = vertex3;

		normals[currentPointsIdx] = calculateVertexNormal(currentFace.firstVertexIndex);
		normals[currentPointsIdx + 1] = calculateVertexNormal(currentFace.secondVertexIndex);
		normals[currentPointsIdx + 2] = calculateVertexNormal(currentFace.thirdVertexIndex);

		currentPointsIdx += 3;


	}
}

void reinitializeArrays() {
	// Reset all arrays
	for(int i = 0; i < 10000; i++) {
		smfVertices[i] = vec4(0,0,0,0);
	}


	vertexFaceMapping.clear();
	smfFaces.clear();
}

void calculateFaceNormal(vec4 vertex1, vec4 vertex2, vec4 vertex3, Face& currentFace) {
		// See p 272
		vec4 U = vertex2 - vertex1;
		vec4 V = vertex3 - vertex2;

		vec4 crossVector = cross(U,V);

		if(debug && std::isnan(crossVector.x)) {
			printf("Invalid cross vector");
			exit(0);
		}

		vec4 normalNormalized = normalize(crossVector);

		vec4 absNormalNormalized = vAbs(normalNormalized);

		double customLength = sqrt(crossVector.x*crossVector.x+crossVector.y*crossVector.y+crossVector.z*crossVector.z);

		vec4 customNormal = crossVector / customLength;

		vec4 absCustomNormal = vAbs(customNormal);

		if(debug) {
			printf("Cross product ");
			printVector(crossVector);
			printf("Normalized vector ");
			printVector(normalNormalized);
			printf("Absolute value vector ");
			printVector(absNormalNormalized);
			printf("Vertex 1 is: %f, %f, %f\n",vertex1.x,vertex1.y,vertex1.z);
			printf("Vertex 2 is: %f, %f, %f\n",vertex2.x,vertex2.y,vertex2.z);
			printf("Vertex 3 is: %f, %f, %f\n",vertex3.x,vertex3.y,vertex3.z);

			printf("Final Color is: %f, %f, %f, %f\n",absNormalNormalized.x,absNormalNormalized.y,absNormalNormalized.z,absNormalNormalized.w);
		}

		currentFace.normal = absCustomNormal;
}


int readSMF(char* fileName, int xOffset) {


	// Read in the SMF file
			std::ifstream infile(fileName);

			char a;
			float b, c, d;
			int numSmfFaces = 0;
			int numSmfVertices = 0;

			while (infile >> a >> b >> c >> d)
			{
				if(a == 'v') {
					smfVertices[numSmfVertices] = vec4(b - xOffset,c,d,1);
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

		return numSmfFaces * 3;
}

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
    glutInitWindowSize( windowWidth, windowHeight );



    mainWindow = glutCreateWindow( "Assignment 8" );
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
	shape1VertexCount = readSMF(fileName,0);
	populatePointsAndNormalsArrays();
	reinitializeArrays();

	shape2VertexCount = readSMF(fileName,-3);
	populatePointsAndNormalsArrays();
	reinitializeArrays();

	shape3VertexCount = readSMF(fileName,3);
	populatePointsAndNormalsArrays();
	reinitializeArrays();

	std::cout << "Press: 1 - To increase camera height" << std::endl;
	std::cout << "Press: 2 - To decrease camera height" << std::endl;
	std::cout << "Press: 3 - To increase camera orbit radius" << std::endl;
	std::cout << "Press: 4 - To decrease camera orbit radius" << std::endl;
	std::cout << "Press: 5 - To increase camera angle (rotate camera)" << std::endl;
	std::cout << "Press: 6 - To decrease camera angle (rotate counterclockwise)" << std::endl;
	std::cout << "Press: z - To reset all values" << std::endl;
	std::cout << "Press: x - To exit the program" << std::endl;


	initMainWindow();

	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc(mouse);

	glEnable(GL_DEPTH_TEST);

	initFrameBuffer();

	glutMainLoop();
	return 0;
}
