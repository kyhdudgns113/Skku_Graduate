#pragma once
/*
*
*
*	x' = x+ - x;
*
*
*	각도 크기 : 180
*	각도 단위 : 2
*	배열 크기 : 90
*
*
*/

#include <cmath>
#include <random>
#include <time.h>

#define G_STDEV_PHYSICS		5.0

class G_STATE {
public:

	int cnt = 0;
	double sum = 0;

	G_STATE();
	~G_STATE();

	double retScore();
	double ret_log();
	void addData(double data);
};

