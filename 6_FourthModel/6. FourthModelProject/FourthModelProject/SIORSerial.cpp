#include "SIORSerial.h"
#include <iostream>
#include <string.h>
/*
m_hComm : CreateFile 로 생성된 변수. 이것을 핸들링하여 시리얼 통신을 하는듯
*/

using namespace std;

SIORSerial::SIORSerial() {

}

SIORSerial::~SIORSerial() {
}

bool SIORSerial::OpenPort(CString portname) {
	m_hComm = CreateFile(L"//./" + portname, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0); //시리얼 포트를 오픈한다. 
	if (m_hComm == INVALID_HANDLE_VALUE)  //정상적으로 포트가 열렸는지 확인
	{
		return false;  //열리지 않았을 경우 false 반환
	}
	else
		return true;   //제대로 열렸을 경우 true 반환
}

void SIORSerial::ClosePort() {
	CloseHandle(m_hComm); //포트를 닫는다.
}


bool SIORSerial::ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity, BYTE Parity, BYTE StopBits) {
	if ((m_bPortReady = GetCommState(m_hComm, &m_dcb)) == 0) { //포트의 상태를 확인. 정상적으로 열리지 않았을 경우 false 반환
		printf("\nGetCommState Error\n");
		//"MessageBox(L, L"Error", MB_OK + MB_ICONERROR);  
		CloseHandle(m_hComm);
		return false;
	}

	//	포트의 대한 기본값을 설정
	//	자세한건 http://purestarman.tistory.com/169 에 나와있다.

	m_dcb.BaudRate = BaudRate;		//	보드레이트
	m_dcb.ByteSize = ByteSize;		//	데이터의 한 단위당 비트수(4~8, 보통은 7~8)
	m_dcb.Parity = Parity;			/*	사용할 패리티
									EVENPARITY	: 짝수 패리티
									MARKPARITY	: 마크 패리티
									NOPARITY	: 없음
									ODDPARITY	: 홀수 패리티
									SPACEPARITY	: 간격 패리티*/
	m_dcb.StopBits = StopBits;		/*	사용될 STOP 비트의 수를 설정한다.
									ONESTOPBIT		: 1비트
									ONE5STOPBITS	: 1.5비트
									TWOSTOPBITS		: 2비트*/
	m_dcb.fBinary = true;			//	바이너리 모드로 할 것인가? 그냥 true 로 하길
	m_dcb.fDsrSensitivity = false;	//	DSR 신호 올때까지 모든 바이트를 무시할지 여부
	m_dcb.fParity = fParity;		//	패리티 체크 활성화 여부
	m_dcb.fOutX = false;			/*	전송을 할 동안 XON/XOFF 흐름 제어 사용 여부
									XonChar 를 받으면 송신, XoffChar 받으면 송신 중지
									*/
	m_dcb.fInX = false;				//	수신을 할 동안 XON/XOFF 흐름 제어를 사용할지 여부
									//	m_dcb.XonChar = 'a';
									//	m_dcb.XoffChar = 'z';

	m_dcb.fNull = false;			//	NULL 바이트를 삭제할지 여부
	m_dcb.fAbortOnError = true;		//	에러발생시 R/W 종료 여부
	m_dcb.fOutxCtsFlow = false;		//	출력을 CTS 신호로 제어할지 여부
	m_dcb.fOutxDsrFlow = false;		//	출력을 DSR 로 제어할지 여부
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxCtsFlow = false;

	m_bPortReady = SetCommState(m_hComm, &m_dcb);  //포트 상태 확인

	if (m_bPortReady == 0) { //포트의 상태를 확인. 정상일 경우 true 반환 아닐 경우 false 반환
							 //MessageBox(L"SetCommState Error");  
		printf("SetCommState Error");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool SIORSerial::SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
	DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
	DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant) {//통신 포트에 관한 Timeout 설정
	if ((m_bPortReady = GetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0)
		return false;

	m_CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout; //통신할때 한바이트가 전송 후 다음 바이트가 전송될때까지의 시간
															  //통신에서 데이터를 읽을 때 Timeout을 사용할 것인지에 대한 여부
	m_CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	//통신에서 데이터를 전송할 때 Timeout을 사용할 것인지에 대한 여부
	m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

	m_bPortReady = SetCommTimeouts(m_hComm, &m_CommTimeouts);  //포트 상태 확인

	if (m_bPortReady == 0) {	//포트 상태가 닫혀 있을 경우 false반환. 아닐 경우 true반환
								//MessageBox(L"StCommTimeouts function failed",L"Com Port Error",MB_OK+MB_ICONERROR);  
		printf("\nStCommTimeouts function failed\n");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool SIORSerial::WriteByte(char bybyte) {
	m_iBytesWritten = 0;

	if (WriteFile(m_hComm, &bybyte, 1, &m_iBytesWritten, NULL) == 0) //입력받은 값을 WriteFile을 통해 포트로 전송한다.
		return false;
	else
		return true;
}

bool SIORSerial::WriteString(std::string &src) {
	int len = src.length();
	this->m_iBytesWritten = 0;

	if (WriteFile(m_hComm, src.c_str(), len, &m_iBytesWritten, NULL) == false)
		return false;
	else
		return true;
}

bool SIORSerial::WriteString(char *src) {
	int len = strlen(src);
	this->m_iBytesWritten = 0;

	if (WriteFile(m_hComm, src, len, &m_iBytesWritten, NULL) == false)
		return false;
	else
		return true;
}

bool SIORSerial::ReadByte(BYTE &resp) {
	BYTE rx;
	resp = 0;

	DWORD dwBytesTransferred = 0;
	if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {//포트에 존재하는 데이터를 ReadFile을 통해 1바이트씩 읽어온다.

		if (dwBytesTransferred == 1) {//데이터를 읽어오는데 성공했을 경우
			resp = rx;  //resp에 데이터를 저장하고 true 반환
			return true;
		}
	}

	return false; //실패했을 경우 false 반환
}

bool SIORSerial::ReadByte(int &resp) {
	BYTE rx;
	resp = 0;

	DWORD dwBytesTransferred = 0;
	if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {//포트에 존재하는 데이터를 ReadFile을 통해 1바이트씩 읽어온다.

		if (dwBytesTransferred == 1) {//데이터를 읽어오는데 성공했을 경우
			resp = rx;  //resp에 데이터를 저장하고 true 반환
			return true;
		}
	}

	return false; //실패했을 경우 false 반환
}

bool SIORSerial::ReadByte(BYTE* &resp, UINT size) {
	DWORD dwBytesTransferred = 0;

	if (ReadFile(m_hComm, resp, size, &dwBytesTransferred, 0)) {
		if (dwBytesTransferred == size)
			return true;
	}


	return false;
}


bool SIORSerial::ReadString(std::string &dest) {
	BYTE rx;
	DWORD dwBytesTransferred = 0;
	char buf[BUFFER_SIZE] = { 0, };
	int idx = 0;

	while (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {
		if (rx == '\r') {

		}
		else if (rx == '\n' || rx == 0 || idx == BUFFER_SIZE - 1)
			break;
		else {
			buf[idx++] = rx;
		}
	}
	dest = buf;
	return true;
}

bool SIORSerial::ReadString(char *dest) {
	BYTE rx;
	DWORD dwBytesTransferred = 0;
	char buf[BUFFER_SIZE] = { 0, };
	int idx = 0;

	while (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {
		if (rx == '\n' || rx == 0 || idx == BUFFER_SIZE - 1)
			break;
		buf[idx++] = rx;
	}

	strcpy(dest, buf);
	return true;
}

bool SIORSerial::Serial(string portName, DWORD baudRate, BYTE byteSize,
	DWORD fParity, BYTE Parity, BYTE stopBits) {

	if (this->OpenPort(portName.c_str()) == false)
		return false;

	this->ConfigurePortW(baudRate, byteSize, fParity, Parity, stopBits);
	this->SetCommunicationTimeouts(0, 0, 0, 0, 0);

	return true;
}

bool SIORSerial::begin(string portName, DWORD baudRate, BYTE byteSize,
	DWORD fParity, BYTE Parity, BYTE stopBits) {
	return this->Serial(portName, baudRate, byteSize, fParity, Parity, stopBits);
}

void SIORSerial::terminate() {
	this->ClosePort();
}

bool SIORSerial::isOpen() {
	return GetCommState(m_hComm, &m_dcb);
}

void SIORSerial::flush() {
	FlushFileBuffers(this->m_hComm);
}