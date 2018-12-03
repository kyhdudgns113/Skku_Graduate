#pragma once
/*
*
*
*	x" = a_m * sin(x) / r
*	x' = x'(0) + x"t
*	x = x(0) + x'(0)t * x"t^2 / 2
*
*	v = v(0) + a_m * t
*
*	Input : a_m = F / m
*	a_r = [a_m * sin(x + 가우시안노이즈) - g * cos(x + 가우시안 노이즈)]
*	x" = a_r / r
*	x' = x'(0) + x"t
*	x = x(0) + x'(0)t + x"t^2 / 2
*
*	v = v(0) + a_m * t - 마찰력
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

