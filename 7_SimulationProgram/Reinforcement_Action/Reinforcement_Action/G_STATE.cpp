
#include "G_STATE.h"



G_STATE::G_STATE() {

}


G_STATE::~G_STATE() {

}

//	각도에 대한 점수를 리턴한다.
//	90도에서 많이 떨어지면 불리하다.
//	180 - |각도 - 90|
double G_STATE::retScore() {
	if (cnt == 0)
		return 0;
	return -abs(this->sum / this->cnt - 90.0);
}

double G_STATE::ret_log() {
	return log2(this->cnt + 2) / (this->cnt + 1);
}

//	각도를 입력받는다.
void G_STATE::addData(double data) {
	this->sum += data;
	this->cnt++;
}