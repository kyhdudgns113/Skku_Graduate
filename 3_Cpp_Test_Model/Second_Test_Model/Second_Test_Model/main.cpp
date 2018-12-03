#pragma warning(disable : 4244)
#pragma warning(disable : 4996)

#include <iostream>
#include <string.h>
#include <string>
#include <random>
#include <cmath>
#include <Windows.h>
#include "SIORSerial.h"

#define 가속도				9.709
#define 반지름				6.9
#define 각도배열크기		90
#define 각속도배열크기					


#define MAX_ANGLE			3600.0
#define BLOCK_ANGLE			20.0			//	1도당 크기
#define SIZE_BLOCK_ANGLE	2.0				//	블록당 몇 도로 할지
#define max_angle			(MAX_ANGLE / (BLOCK_ANGLE * SIZE_BLOCK_ANGLE))

#define MAX_SPEED			512.0
#define BLOCK_SPEED			1.0
#define SIZE_BLOCK_SPEED	4.0				//	 최소 2
#define max_speed			(MAX_SPEED / (BLOCK_SPEED * SIZE_BLOCK_SPEED))

#define G_DELTA_CHOOSE		5.0


typedef class node {
public:
	double cnt = 0, sum = 0;

	void addData(double data) {
		this->cnt++;
		this->sum += data;
	}

	int retVal() {
		if (cnt == 0)
			return -100;
		else if (sum / cnt < 0)
			return max_angle / 2;
		else if (sum / cnt > max_angle)
			return max_angle / 2;
		return abs(sum / cnt - max_angle / 2);
	}

	double retK() {
		return log2(this->cnt + 2) / (this->cnt + 1);
	}

	double retFinal(double _gauss) {
		double ret = 0;

		if (cnt == 0)
			ret = -100;
		else if (sum / cnt < 0)
			ret = max_angle / 2;
		else if (sum / cnt > max_angle)
			ret = max_angle / 2;
		else
			ret = abs(sum / cnt - max_angle / 2);

		_gauss *= log2(this->cnt + 2) / (this->cnt + 1);
		_gauss += ret;

		return _gauss;
	}
}NODE;

using namespace std;

void read_file();
void write_file();
void write_file_number();


LARGE_INTEGER bef, aft, freq, time_begin;

NODE NEXT_STATE[90][128];

SIORSerial ser;
char chrBuffer[BUFFER_SIZE];

string strBuffer;
BYTE write_buffer, input_buffer;

default_random_engine generator;
normal_distribution<double> distribution(0.0, G_DELTA_CHOOSE);
normal_distribution<double> make_gaussian_noise(0.0, 1.5);

int cnt_study = 0, cnt_loop = 0, read_end = 0;
int previous_angle, present_angle;
int previous_speed = -1, present_speed;
int median_angle = max_angle / 2, median_speed = max_speed / 2;


int main() {
	QueryPerformanceCounter(&time_begin);
	printf("Start program\n");
	int i = 0, j = 0;
	ser.begin("COM6", 115200);

	printf("Reading File..."), read_file(), cout << " Completed!" << endl;

	QueryPerformanceCounter(&bef);
	QueryPerformanceFrequency(&freq);
	printf("Serial start\n");

	while (ser.isOpen()) {
		printf("a ");
		read_end = 0;
		//	Read statement, present angle
		int input_signal = -1;
		while (input_signal == -1) {
			ser.ReadString(strBuffer);
			printf("result is %s\n", strBuffer.c_str());
			if (strBuffer == "END") {
				printf(" END ");
				read_end = true;
				break;
			}
			input_signal = atoi(strBuffer.c_str());
		}

		//	Error checking
		//	각도의 범위가 벗어났을때, 다시 이전으로 돌아와서 다른 값을 대입하여 학습한다.

		if (read_end) {
			NEXT_STATE[previous_angle][present_speed].addData(max_angle);
			present_angle = previous_angle;
		}
		else {
			present_angle = input_signal;

			if (cnt_loop) {
				double temp_gaussian = 0;
				temp_gaussian = make_gaussian_noise(generator);
				NEXT_STATE[previous_angle][present_speed].addData(temp_gaussian + present_angle);
			}

		}
		printf("(%4d, %4d) : %4d\n", previous_angle, previous_speed, present_angle);
		double best_value = 999999;
		int best_i = -1;

		//	Select action
		//	가우시안 랜덤 변수를 이용하여 댜앙한 값들을 학습하도록 한다.
		for (i = 0; i < max_speed; i++) {
			double temp_gauss = distribution(generator);
			temp_gauss = NEXT_STATE[present_angle][i].retFinal(temp_gauss);

			if (best_value > temp_gauss) {
				best_value = temp_gauss;
				best_i = i;
			}
		}
		//	printf(" c ");
		previous_speed = present_speed;
		present_speed = best_i;
		previous_angle = present_angle;

		//	BYTE 자료형은 buf 에 데이터를 저장해서 넘긴다.
		BYTE buf = present_speed;
		ser.WriteByte(buf);
		//	printf("d \n");
		cnt_loop++;
		for (i = 1; i < 5; i++) {
			if (cnt_loop % 5000 == 1000 * i)
				printf(".");
		}
		if (cnt_loop % 5000 == 0) {
			printf("cnt is %d, ", cnt_loop);
			//	write_file();
			QueryPerformanceCounter(&aft);
			printf("%.2lf seconds\n", (double)(aft.QuadPart - bef.QuadPart) / (double)freq.QuadPart);
			bef = aft;
		}
	}

	printf("End Transmission\n");
	write_file();
	write_file_number();
	printf("End Filing\n");

	printf("Program terminated. ");
	QueryPerformanceCounter(&aft);

	double result_time = (double)(aft.QuadPart - time_begin.QuadPart) / (double)freq.QuadPart;
	printf("%.2lf seconds in %d targets\n", result_time, cnt_loop);
	printf("average time by 5000 data %.3lf\n", result_time / cnt_loop * 5000);
	
}


void read_file() {
	int i, j, k;
	FILE *fp;
	fp = fopen("input.txt", "r");
	if (fp == nullptr) {
		printf("파일이 없습니다. 새로 만듭니다. \n");
		return;
	}
	for (i = 0; i < max_angle; i++) {
		for (j = 0; j < max_speed; j++) {
			fscanf(fp, " %d %d", &NEXT_STATE[i][j].cnt, &NEXT_STATE[i][j].sum);
		}
	}
	fscanf(fp, "%d", &cnt_study);
	fclose(fp);
}

void write_file() {
	int i, j, k, cnt = 0;
	FILE *fp;

	fp = fopen("input.txt", "w");
	for (i = 0; i < max_angle; i++) {
		for (j = 0; j < max_speed; j++) {
			if (NEXT_STATE[i][j].cnt == 0)
				cnt++;
			fprintf(fp, " %d %d", int(NEXT_STATE[i][j].cnt), int(NEXT_STATE[i][j].sum));

		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "%d\n", cnt_study + cnt_loop);
	fprintf(fp, "%d", cnt);
	fclose(fp);
}

void write_file_number() {
	int i, j, k, cnt = 0;
	FILE *fp;
	string a = "input_";
	string b = itoa(cnt_study + cnt_loop, chrBuffer, 10);
	a = a + b;
	a = a + ".txt";
	fp = fopen(a.c_str(), "w");
	for (i = 0; i < max_angle; i++) {
		for (j = 0; j < max_speed; j++) {
			if (NEXT_STATE[i][j].cnt == 0)
				cnt++;
			fprintf(fp, " %2d %4d", int(NEXT_STATE[i][j].cnt), int(NEXT_STATE[i][j].sum));
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "%d\n", cnt_study + cnt_loop);
	fprintf(fp, "%d", cnt);
	fclose(fp);
}