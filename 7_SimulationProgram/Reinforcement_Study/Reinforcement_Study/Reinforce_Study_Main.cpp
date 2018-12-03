#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <iostream>
#include <Windows.h>
#include <algorithm>
#include "G_STATE.h"
#include "R_Study_main.h"

using namespace std;


unsigned int seed = static_cast<unsigned int>(time(NULL));
std::normal_distribution<double> NOISE_PHYSICS(0.0, G_STDEV_PHYSICS);
std::normal_distribution<double> NOISE_PHYSICS2(0.0, 2.0);		//	이건 action 에서 쓰인다.
std::default_random_engine generator(seed);

void main_study();
void main_action();

int main() {

	int main_input = -1;
	int i, temp_best_action;
	int same_repeat = 0;


	read_file();

	while (1) {
		LARGE_INTEGER tb, ta, tf;

		QueryPerformanceCounter(&tb);
		QueryPerformanceFrequency(&tf);
		while (1) {

			double a_m = 0;
			double input_angle = 0;
			double temp_ddx = 0;
			double temp_x = G_START_ANGLE;
			double temp_dx = 0;

			int i_t_x = 0;
			int i_tdx = 0;


			if (cnt_loop == 0) {
				//	printf("tx = %4.2lf, tdx = %4.2lf, x = %4.2lf, dx = %4.2lf, (%d, %d)\n", temp_x, temp_dx, x, dx, iv, ipv);
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
			else if (same_repeat > G_SPEED_SIZE) {

				same_repeat = 0;

				x = (double)(rand() % 160 + 10);
				px = x;
				dx = 0;
				pdx = 0;

				iv = 0;
				ipv = 0;

				ix = ret_idx_angle(x);
				idx = ret_idx_dangle(dx);

				ipx = ret_idx_angle(px);
				ipdx = ret_idx_dangle(pdx);
			}
			else {
				a_m = (double)(iv - ipv) / G_DT;
				temp_ddx = (a_m * sin(x) - G_GRAVITY * cos(x)) / G_RADIUS;
				temp_x = x + dx * G_DT + temp_ddx * G_DT * G_DT / 2 + NOISE_SENSOR(generator);
				temp_dx = dx + temp_ddx * G_DT;

				
				if (temp_x > G_D_ANGLE_MAX - 5.0 || temp_x < G_D_ANGLE_MIN + 5.0 ||
					temp_dx > G_D_DANGLE_MAX || temp_dx < G_D_DANGLE_MIN) {

					g_state[ix][idx][ipv][iv].addData(temp_x > 0 ? 180.0 : 0.0);
					same_repeat++;
					iv = ipv;
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

			double temp_best_score = -10000;
			for (i = 0; i < G_SPEED_SIZE; i++) {
				double temp_noise = NOISE_PHYSICS(generator);
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


			//	Ma_Moo_Ri
			cnt_loop++;

			for (i = 1; i < 10; i++) {
				if (cnt_loop % 10000000 == 1000000 * i) {
					printf(".");
				}
			}

			if (cnt_loop % 10000000 == 0) {
				printf("%d number studied ", cnt_loop);
				write_file_number();
				QueryPerformanceCounter(&ta);
				printf("%.2lf seconds.\n", (double)(ta.QuadPart - tb.QuadPart) / (tf.QuadPart));
				QueryPerformanceCounter(&tb);
				QueryPerformanceFrequency(&tf);
			}
		}
	}
}
