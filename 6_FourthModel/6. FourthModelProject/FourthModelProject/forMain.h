#pragma once
#include "G_STATE.h"
#include <random>
#include <time.h>
#include <iostream>

using namespace std;

#define G_PI				3.1415926535
#define G_GRAVITY			9.8
#define G_START_ANGLE		90
#define G_RADIUS			0.1
#define G_STDEV_SENSOR		2.0
//#define G_STDEV_PHYSICS		5.0		//defined in "G_STATE.h"
#define G_DT				0.1
#define G_STD_SPEED			1.0

#define G_ANGLE_MAX		180
#define G_ANGLE_MIN		0
#define G_ANGLE_BLOCK	2
#define G_ANGLE_SIZE	((G_ANGLE_MAX - G_ANGLE_MIN) / G_ANGLE_BLOCK + 1)
#define G_D_ANGLE_MAX	(double)G_ANGLE_MAX
#define G_D_ANGLE_MIN	(double)G_ANGLE_MIN

#define G_DANGLE_MAX	60
#define G_DANGLE_MIN	(-60)
#define G_DANGLE_BLOCK	1
#define G_DANGLE_SIZE	((G_DANGLE_MAX - G_DANGLE_MIN) / G_DANGLE_BLOCK + 1)
#define G_D_DANGLE_MAX	(double)G_DANGLE_MAX
#define G_D_DANGLE_MIN	(double)G_DANGLE_MIN

#define G_SPEED_MAX		32
#define G_SPEED_MIN		(-32)
#define G_SPEED_BLOCK	1
#define G_SPEED_SIZE	((G_SPEED_MAX - G_SPEED_MIN) / G_SPEED_BLOCK + 1)

G_STATE g_state[G_ANGLE_SIZE][G_DANGLE_SIZE][G_SPEED_SIZE][G_SPEED_SIZE];

double x = G_START_ANGLE, dx, ddx, v;
double px, pdx, pddx;

int ix, idx, iddx, iv = (G_SPEED_MAX + G_SPEED_MIN) / 2, iaction;
int ipx, ipdx, ipddx, ipv;
int cnt_loop, cnt_study;


normal_distribution<double> NOISE_SENSOR(0.0, G_STDEV_SENSOR);

void read_file();
void write_file();
void write_file_number();

int ret_idx_angle(double _x);
int ret_idx_dangle(double _dx);
int ret_idx_speed(double _v);



void read_file() {
	FILE *fp = fopen("input_200000000.txt", "r");
	if (fp == nullptr) {
		printf("파일이 없습니다. \n");
		return;
	}

	int i, j, k, l;
	printf("파일을 읽습니다");
	for (i = 0; i < G_ANGLE_SIZE; i++) {
		for (j = 0; j < 10; j++) {
			if (i % G_ANGLE_SIZE == j * 9)
				printf(";");
		}
		for (j = 0; j < G_DANGLE_SIZE; j++) {
			for (k = 0; k < G_SPEED_SIZE; k++) {
				for (l = 0; l < G_SPEED_SIZE; l++) {
					fscanf(fp, "%d %lf", &g_state[i][j][k][l].cnt, &g_state[i][j][k][l].sum);
				}
			}
		}
	}
	printf("종료\n");
	fscanf(fp, "%d", &cnt_study);
	fclose(fp);
}

void write_file() {
	FILE *fp = fopen("input.txt", "w");

	int i, j, k, l;
	int cnt_zero = 0;

	for (i = 0; i < G_ANGLE_SIZE; i++) {
		for (j = 1; j < 10; j++) {
			if (i * j % (G_ANGLE_SIZE / 10) == 0)
				printf(".");
		}
		for (j = 0; j < G_DANGLE_SIZE; j++) {
			for (k = 0; k < G_SPEED_SIZE; k++) {
				for (l = 0; l < G_SPEED_SIZE; l++) {
					if (g_state[i][j][k][l].cnt == 0)
						cnt_zero++;
					fprintf(fp, "%3d %5d ", g_state[i][j][k][l].cnt, (int)g_state[i][j][k][l].sum);
				}
				fprintf(fp, "\n");
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "%d\n%d\n", cnt_study + cnt_loop, cnt_zero);
	fclose(fp);
}

void write_file_number() {

	char buf[128];

	string a = "input_";
	string b = itoa(cnt_loop + cnt_study, buf, 10);
	a = a + b + ".txt";
	FILE *fp = fopen(a.c_str(), "w");

	int i, j, k, l;
	int cnt_zero = 0;

	for (i = 0; i < G_ANGLE_SIZE; i++) {
		for (j = 0; j < 10; j++) {
			if (i % G_ANGLE_SIZE == j * 9)
				printf(";");
		}
		for (j = 0; j < G_DANGLE_SIZE; j++) {
			for (k = 0; k < G_SPEED_SIZE; k++) {
				for (l = 0; l < G_SPEED_SIZE; l++) {
					if (g_state[i][j][k][l].cnt == 0)
						cnt_zero++;
					fprintf(fp, "%4d %5d ", g_state[i][j][k][l].cnt, (int)g_state[i][j][k][l].sum);
				}
				fprintf(fp, "\n");
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "%d\n%d\n", cnt_study + cnt_loop, cnt_zero);

	fclose(fp);
}

int ret_idx_angle(double _x) {

	if (_x > G_ANGLE_MAX || _x < G_ANGLE_MIN) {
		return -1;
	}

	double ret_d = _x - G_ANGLE_MIN;
	int ret_i = ret_d;

	return ret_i / G_ANGLE_BLOCK;
}

int ret_idx_dangle(double _dx) {
	if (_dx > G_DANGLE_MAX || _dx < G_DANGLE_MIN)
		return -1;

	return (_dx - G_DANGLE_MIN) / G_DANGLE_BLOCK;
}

int ret_idx_speed(double _v) {
	if (_v > G_SPEED_MAX || _v < G_SPEED_MIN)
		return -1;

	return (_v - G_SPEED_MIN) / G_SPEED_BLOCK;
}
