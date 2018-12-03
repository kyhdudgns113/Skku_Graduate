#include "SIORSerial.h"
#include <iostream>

using namespace std;


int main() {
	SIORSerial ser;
	ser.begin("COM6", 115200);

	char buf[BUFFER_SIZE] = { 0, };
	string input_signal;
	input_signal = ser.ReadString(buf);


	ser.ClosePort();
}