#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <iostream>
#include <algorithm>
#include "SIORSerial.h"
#include "forMain.h"

using namespace std;

SIORSerial ser;
string strBuffer;

unsigned int seed = static_cast<unsigned int>(time(NULL));
std::normal_distribution<double> NOISE_PHYSICS(0.0, G_STDEV_PHYSICS);
std::default_random_engine generator(seed);


int main() {
	int main_input = -1;
	int i, temp_best_action;
	int same_repeat = 0;

	ser.begin("COM6", 115200);

	read_file();
	printf("시리얼 통신?\n");
	while (ser.isOpen() == true) {
		LARGE_INTEGER tb, ta, tf;

		QueryPerformanceCounter(&tb);
		QueryPerformanceFrequency(&tf);

		double input_angle = 0;
		double temp_x = G_START_ANGLE;
		double temp_dx = 0;

		int input_signal = -1;

		while (input_signal == -1) {
			ser.ReadString(strBuffer);
			printf("result is %s\n", strBuffer.c_str());
			if (strBuffer == "END") {
				printf(" END ");
				break;
			}
			input_signal = atoi(strBuffer.c_str());
		}

		int i_t_x = 0;
		int i_tdx = 0;


		if (cnt_loop == 0) {
			//	printf("tx = %4.2lf, tdx = %4.2lf, x = %4.2lf, dx = %4.2lf, (%d, %d)\n", temp_x, temp_dx, x, dx, iv, ipv);


			iv = 0;
			ipv = 0;

			ix = input_signal;
			idx = 0;

			ipx = ix;
			ipdx = 0;
		}
		else {
			temp_dx = x - px;

			printf("(%6.2lf, %6.2lf, %2d) -> (%6.2lf, %7.2lf, %2d) ", x, dx, ipv, temp_x, temp_dx, iv);

			if (temp_x > G_D_ANGLE_MAX - 5.0 || temp_x < G_D_ANGLE_MIN + 5.0 ||
				temp_dx > G_D_DANGLE_MAX || temp_dx < G_D_DANGLE_MIN) {
				printf(" 꿍\n");

				g_state[ix][idx][ipv][iv].addData(temp_x > 0 ? 180.0 : 0.0);

			}
			else {

				g_state[ix][idx][ipv][iv].addData(temp_x);

				px = x;
				pdx = dx;
				x = temp_x;
				dx = temp_dx;

				ipv = iv;
				ipx = ret_idx_angle(px);
				ipdx = ret_idx_dangle(pdx);

				ix = ret_idx_angle(x);
				idx = ret_idx_dangle(dx);
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
			if (cnt_loop % 1000 == 100 * i) {
				printf(".");
			}
		}

		if (cnt_loop % 1000 == 0) {
			printf("%d number studied ", cnt_loop);
			write_file_number();
			QueryPerformanceCounter(&ta);
			printf("%.2lf seconds.\n", (double)(ta.QuadPart - tb.QuadPart) / (tf.QuadPart));
			QueryPerformanceCounter(&tb);
			QueryPerformanceFrequency(&tf);
		}
	}
}

