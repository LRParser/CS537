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

const int N = 10;
const int uRange = N;
const int vRange = N;
vec4 quads[N][N];


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


// Take 4 control points, and u, provide output
vec3 calcPoint(float u, vec3 controlPoints[4]) {

	vec3 retVal;
	float uu = (1 - u);

	retVal.x = uu * uu * uu * controlPoints[0].x
	+ 3 * u * uu * uu * controlPoints[1].x
	+ 3 * u * u * uu * controlPoints[2].x
	+ u * u * u * controlPoints[3].x;

	retVal.y = uu * uu * uu * controlPoints[0].y
	+ 3 * u * uu * uu * controlPoints[1].y
	+ 3 * u * u * uu * controlPoints[2].y
	+ u * u * u * controlPoints[3].y;

	return retVal;
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

					float bernsteinForJ = getBernsteinFactor(vParam,j);

					float controlX = patch[i][j][0];
					float controlY = patch[i][j][1];
					float controlZ = patch[i][j][2];
					vec4 controlPoint = vec4(controlX,controlY,controlZ,0);
					float weight = bernsteinForU * bernsteinForJ;
					pointSum += weight * controlPoint;

				}
			}

			// printf("v %f %f %f\n",pointSum.x,pointSum.y,pointSum.z);

			quads[u][v] = pointSum;

			// printf("%f, %f\n",pointSum.x,pointSum.y);


		}

	}
}

// Print vertex in SMF format
void printVertex(vec4 vertex) {
	printf("v %f %f %f\n",vertex.x,vertex.y,vertex.z);
}

int
main( int argc, char **argv )
{
	calcPatchPoints();

	int faceNum = 1;
	int vertexNum = 1;

	// Iterate thru i,j - e.g., u,v
	for(int i = 0; i < N - 1; i++) {

		for(int j=0; j < N - 1; j++) {

			// First triangle
			vec4 vertex1 =  quads[i][j];
			vec4 vertex2 = quads[i+1][j];
			vec4 vertex3 = quads[j+1][i];

			// Second triangle
			vec4 vertex4 = quads[i+1][j];
			vec4 vertex5 = quads[j+1][i+1];
			vec4 vertex6 = quads[i][j+1];

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



