#pragma once
/*
*
*
*	x' = x+ - x;
*
*
*	���� ũ�� : 180
*	���� ���� : 2
*	�迭 ũ�� : 90
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

