/*
 * bezierCurve.cpp
 *
 *  Created on: Mar 1, 2017
 *      Author: josephheenan
 */

#include "Angel.h"
#include <math.h>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#define DEBUG 1

const int N = 20;
const int uRange = N;
const int vRange = N;

vec4 vertices[10000];
vec4 patch[4][4];
int totalRead = 0;

vec4 interpolatedPoints[N][N];

void printVertex(vec4 vertex) {
	printf("v %f %f %f\n",vertex.x,vertex.y,vertex.z);
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

int printPatchCorners(int startVertexNum, vec4 patch[4][4]) {

	printVertex(patch[0][0]);
	printVertex(patch[3][0]);
	printVertex(patch[3][3]);

	// Print face info for triangle 1
	printf("f %d %d %d\n",startVertexNum++,startVertexNum++,startVertexNum++);

	printVertex(patch[0][0]);
	printVertex(patch[3][3]);
	printVertex(patch[0][3]);
	// Print face info for triangle 2
	printf("f %d %d %d\n",startVertexNum++,startVertexNum++,startVertexNum++);

	return startVertexNum;
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

// Print vertex in SMF format
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
}

int
main( int argc, char **argv )
{

	readPatchFile("patchPoints.txt");

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

	// Print all interpolated points
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

				printVertex(vertex1);
				printVertex(vertex2);
				printVertex(vertex3);

				// Print face info for triangle 1
				printf("f %d %d %d\n",vertexNum++,vertexNum++,vertexNum++);

				printVertex(vertex4);
				printVertex(vertex5);
				printVertex(vertex6);
				// Print face info for triangle 2
				printf("f %d %d %d\n",vertexNum++,vertexNum++,vertexNum++);


			}
		}

	return 0;
}



