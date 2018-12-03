#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <iostream>
#include <algorithm>
#include "forMain.h"
#include <string>
#include <Windows.h>

using namespace std;



unsigned int seed = static_cast<unsigned int>(time(NULL));
std::normal_distribution<double> NOISE_PHYSICS(0.0, G_STDEV_PHYSICS);
std::normal_distribution<double> NOISE_PHYSICS2(0.0, 2.0);		//	이건 action 에서 쓰인다.
std::default_random_engine generator(seed);


int main() {
	int main_input = -1;
	string string_input;
	int i, temp_best_action;
	int same_repeat = 0;


	read_file();

	double a_m = 0;
	double input_angle = 0;
	double temp_ddx = 0;
	double temp_x = G_START_ANGLE;
	double temp_dx = 0;

	while (main_input || string_input != "NO") {

		while (1) {

			if (cnt_loop == 0) {

				x = temp_x;
				dx = temp_dx;
				ddx = temp_ddx;
				px = x;
				pdx = dx;
				pddx = ddx;

				iv = 0;
				ipv = 0;
				ix = ret_idx_angle(x);
				idx = ret_idx_dangle(dx);
				iddx = ddx;

				ipx = ret_idx_angle(px);
				ipdx = ret_idx_dangle(pdx);
				ipddx = pddx;
			}
			else {
				a_m = (double)(iv - ipv) / G_DT;
				temp_ddx = (a_m * sin(x) - G_GRAVITY * cos(x)) / G_RADIUS;
				temp_x = x + dx * G_DT + temp_ddx * G_DT * G_DT / 2 + NOISE_SENSOR(generator);
				temp_dx = dx + temp_ddx * G_DT;
				if (cnt_loop % 10 == 0)
					printf("\n(이전각도, 각속도, 속도)  ->  ( 각도, 속도)\n");
				printf("(%8.2lf, %6.2lf, %4d)  ->  (%5.2lf, %4d) ", x, dx, ipv, temp_x, iv);

				if (temp_x > G_D_ANGLE_MAX - 5.0 || temp_x < G_D_ANGLE_MIN + 5.0) {
					printf(" 넘어졌다. %d 번 움직였다. \n", cnt_loop);
					break;
				}
				else if (temp_dx > G_D_DANGLE_MAX || temp_dx < G_D_DANGLE_MIN) {
					printf(" 너무 빨라서 넘어졌다. %d 번 움직였다.\n", cnt_loop);
					break;
				}
				else {
					same_repeat = 0;

					g_state[ix][idx][ipv][iv].addData(temp_x);

					px = x;
					pdx = dx;
					pddx = ddx;
					x = temp_x;
					dx = temp_dx;
					ddx = temp_ddx;

					ipv = iv;
					ipx = ret_idx_angle(px);
					ipdx = ret_idx_dangle(pdx);
					ipddx = pddx;
					ix = ret_idx_angle(x);
					idx = ret_idx_dangle(dx);
					iddx = ddx;
				}
			}
			printf("\n");
			double temp_best_score = -10000;

			for (i = 0; i < G_SPEED_SIZE; i++) {
				double temp_noise = NOISE_PHYSICS2(generator);
				temp_noise *= g_state[ix][idx][iv][i].ret_log();
				double temp_score = g_state[ix][idx][iv][i].retScore();
				temp_score += temp_noise;

				if (temp_score > temp_best_score) {
					temp_best_score = temp_score;
					temp_best_action = i;
				}
			}
			ipv = iv;
			iv = temp_best_action;

			cnt_loop++;
			Sleep(300);
		}

		printf("더 진행하시겠습니까? : ");
		cin >> string_input;

		main_input = 0;

		if (string_input == "No" || string_input == "no" ||
			string_input == "nO" || string_input[0] == '0')
			string_input = "NO";

		temp_x = (double)(rand() % 160 + 10);
		temp_dx = 0;
		iv = ipv = 0;
		temp_ddx = G_GRAVITY * cos(x) / G_RADIUS;
		cnt_loop = 0;
	}
}

