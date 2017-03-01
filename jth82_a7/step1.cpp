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


mat4 B = mat4(1,-3,3,-1,0,3,-6,3,0,0,3,-1,0,0,0,1);
mat4 Btranspose = transpose(B);

vec3 controlPoints[4] = {
		vec3(0.0,0.0,0.0),
		vec3(.25f,1.0,0.0),
		vec3(.7f,.9f,0.0),
		vec3(1.0f,0.0f)
};

// Take 4 control points, and u, provide output
vec3 calcPoint(float u) {
	// Sum of 4 values

	vec3 val1 = (1 - u) * (1 - u) * (1 - u) * controlPoints[0];
	vec3 val2 = 3 * u * (1 - u) * (1 - u) * controlPoints[1];
	vec3 val3 = 3 * u * u * (1 - u) * * controlPoints[2];
	vec3 val4 = u * u * u * controlPoints[3];
	vec3 sum = val1 + val2 + val3 + val4;

	return sum;

}



int
main( int argc, char **argv )
{

	// Sample at 20 points along curve in range [0..1]
	for(int i = 1; i < 21; i++) {
		float u = (float)i/20;
		vec3 y = calcPoint(u);
		printf("%f,%f,%f,%f\n",u,y.x,y.y,y.z);
	}

	return 0;
}



