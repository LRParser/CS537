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


mat4 B = mat4(1,-3,3,-1,0,3,-6,3,0,0,3,-1,0,0,0,1);
mat4 Btranspose = transpose(B);

float p[4][2] = {
		{0.0,0.0},
		{.25,1.0},
		{.7,.9},
		{1.0,0.0}
};


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

vec3 patchVec[16] = {
	vec3( 0.0, 0.0, 0.0 ),
		vec3( 2.0, 0.0, 1.5),
		vec3( 4.0, 0.0, 2.9 ),
		vec3( 6.0, 0.0, 0.0 ),
		vec3( 0.0, 2.0, 1.1 ),
		vec3( 2.0, 2.0, 3.9 ),
		vec3( 4.0, 2.0, 3.1 ),
		vec3( 6.0, 2.0, 0.7 ),
		vec3( 0.0, 4.0, -0.5),
		vec3( 2.0, 4.0, 2.6 ),
		vec3( 4.0, 4.0, 2.4 ),
		vec3( 6.0, 4.0, 0.4 ),
		vec3( 0.0, 6.0, 0.3 ),
		vec3( 2.0, 6.0, -1.1),
		vec3( 4.0, 6.0, 1.3 ),
		vec3( 6.0, 6.0, -0.2)
};

vec3 cpVec1[4] = {
	vec3( 0.0, 0.0, 0.0 ),
		vec3( 2.0, 0.0, 1.5),
		vec3( 4.0, 0.0, 2.9 ),
		vec3( 6.0, 0.0, 0.0 ),

};

vec4 cpVec2[4] = {
				vec3( 0.0, 2.0, 1.1 ),
				vec3( 2.0, 2.0, 3.9 ),
				vec3( 4.0, 2.0, 3.1 ),
				vec3( 6.0, 2.0, 0.7 )

};

vec4 cpVec3[4] = {
						vec3( 0.0, 4.0, -0.5),
						vec3( 2.0, 4.0, 2.6 ),
						vec3( 4.0, 4.0, 2.4 ),
						vec3( 6.0, 4.0, 0.4 )
};

vec4 cpVec4[4] = {
		vec3( 0.0, 6.0, 0.3 ),
		vec3( 2.0, 6.0, -1.1),
		vec3( 4.0, 6.0, 1.3 ),
		vec3( 6.0, 6.0, -0.2)
};




vec4 getBernsteinFactors(float u) {
	float uu = 1-u;
	float val1 = uu * uu * uu;
	float val2 = 3 * u * uu * uu;
	float val3 = 3 * u * u * uu;
	float val4 = u * u * u;
	vec4 returnVec = vec4();
	returnVec[0] = val1;
	returnVec[1] = val2;
	returnVec[2] = val3;
	returnVec[3] = val4;
	return returnVec;
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


vec4 calcPatchPoints() {

	for(int u = 0; u < 1; u++) {

		float uParam = u / 10;

		for(int v = 0; v < 1; v++) {

			float vParam = v / 10;
			vec4 bernsteinsForU = getBernsteinFactors(u);
			vec4 bernsteinsForV = getBernsteinFactors(v);

			for(int i = 0; i < 4; i++) {

				for(int j = 0; j < 4; j++) {

					float controlX = patch[i][j][0];
					float controlY = patch[i][j][1];
					float controlZ = patch[i][j][2];
					vec4 controlPoint = vec4(controlX,controlY,controlZ,0);
					vec4 newPoint = dot(dot(bernsteinsForU,bernsteinsForV),controlPoint);
					printf("v %f %f %f\n",newPoint.x,newPoint.y,newPoint.z);
				}
			}

		}

	}




}



vec3 calcPoint2() {
	float N = 4;
	float d = 1.0/(N-1.0);
	float u, uu;
	for(int i = 0; i < N; i++) {

		u = i*d;
		uu = 1.0-u;

		float x = p[0][0]*uu*uu*uu
				+ 3.0*p[1][0]*uu*uu*u
				+ 3.0*p[2][0]*uu*u*u
				+ p[3][0]*u*u*u;

		float y = p[0][1]*uu*uu*uu
				+ 3.0*p[1][1]*uu*uu*u
				+ 3.0*p[2][1]*uu*u*u
				+ p[3][1]*u*u*u;

		printf("%f,%f\n",x,y);

	}
}


float points[4][4];

void calc3() {
	int N = 4;
	float d = 1.0/(N-1.0);  // Or 3
	float u, uu;
	for(int i=0;i<N;i++) {
		u = i*d;
		uu = 1.0 - u;
		for(int j = 0; j < 2; j++) {
			points[i][j] = (float) p[0][j] * uu * uu * uu
					+ 3.0 * p[1][j] * uu * uu * u
					+ 3.0 * p[2][j] * uu *u*u
					+ p[3][j] * u * u * u;
			printf("%f\n",points[i][j]);
		}
	}

}



void generateBernsteins() {

}


int
main( int argc, char **argv )
{

	/*
	// Sample at 20 points along curve in range [0..1]
	vec3 controlPoints[4] = {
			vec3(0.0,0.0,0.0),
			vec3(.25f,1.0,0.0),
			vec3(.7f,.9f,0.0),
			vec3(1.0f,0.0f,0.0)
	};

	for(int i = 1; i < 21; i++) {
		float u = (float)i/20;
		vec3 y = calcPoint(u,controlPoints);
		printf("%f,%f\n",y.x,y.y);
	}


	printf("Try 2\n");

	calcPoint2();

	printf("Try 3\n");

	calc3();

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 2; j++) {
			printf("%f,%f\n",points[i][j]);
		}
	}
	*/
	calcPatchPoints();


	return 0;
}



