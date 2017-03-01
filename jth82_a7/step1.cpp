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

int
main( int argc, char **argv )
{



	/* Working code for Bezier Curve calculation
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
	*/

	calcPatchPoints();


	return 0;
}



