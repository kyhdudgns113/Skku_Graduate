
#include "G_STATE.h"



G_STATE::G_STATE() {

}


G_STATE::~G_STATE() {

}

//	������ ���� ������ �����Ѵ�.
//	90������ ���� �������� �Ҹ��ϴ�.
//	180 - |���� - 90|
double G_STATE::retScore() {
	if (cnt == 0)
		return 0;
	return -abs(this->sum / this->cnt - 90.0);
}

double G_STATE::ret_log() {
	return log2(this->cnt + 2) / (this->cnt + 1);
}

//	������ �Է¹޴´�.
void G_STATE::addData(double data) {
	this->sum += data;
	this->cnt++;
}