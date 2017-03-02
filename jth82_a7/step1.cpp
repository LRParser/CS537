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
					vec4 controlPoint = vec4(controlX,controlY,controlZ,0);
					float weight = bernsteinForU * bernsteinForJ;
					pointSum += weight * controlPoint;

				}
			}

			printf("v %f %f %f\n",pointSum.x,pointSum.y,pointSum.z);


		}

	}
}

int
main( int argc, char **argv )
{





	calcPatchPoints();


	return 0;
}



