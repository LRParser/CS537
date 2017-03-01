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

vec3 controlPoints[4] = {
		vec3(0.0,0.0,0.0),
		vec3(.25f,1.0,0.0),
		vec3(.7f,.9f,0.0),
		vec3(1.0f,0.0f,0.0)
};


// Take 4 control points, and u, provide output
vec3 calcPoint(float u) {

	vec3 retVal;
	float uu = (1 - u);

	//printf("xs are: %f, %f, %f, %f\n",controlPoints[0].x,controlPoints[1].x,controlPoints[2].x,controlPoints[3].x);
	//printf("ys are: %f, %f, %f, %f\n",controlPoints[0].y,controlPoints[1].y,controlPoints[2].y,controlPoints[3].y);

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


int
main( int argc, char **argv )
{

	// Sample at 20 points along curve in range [0..1]

	for(int i = 1; i < 21; i++) {
		float u = (float)i/20;
		vec3 y = calcPoint(u);
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


	return 0;
}



