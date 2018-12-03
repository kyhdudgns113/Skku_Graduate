#pragma warning(disable:4996)
#pragma warning(disable:4244)
#include <iostream>
#include <Windows.h>
#include <random>
#include <time.h>

#define STD_DEV		2.0

using namespace std;

default_random_engine g_engine(time(nullptr));

int main() {
	int n, i;
	double std_dev = 0;

	printf("How many do you want to print? : ");
	scanf("%d", &n);

	while (n > 0) {
		printf("Standard Deviation : ");
		scanf("%lf", &std_dev);

		normal_distribution<double> g_gauss_noise(0.0, std_dev);

		for (i = 0; i < n; i++) {
			printf("%3d is %9lf\n", i, g_gauss_noise(g_engine));
		}

		printf("How many do you want to print? : ");
		scanf("%d", &n);
	}
	
}