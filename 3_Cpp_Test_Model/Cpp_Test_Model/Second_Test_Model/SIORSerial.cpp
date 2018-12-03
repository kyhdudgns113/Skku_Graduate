#include "SIORSerial.h"
#include <iostream>
#include <string.h>
/*
m_hComm : CreateFile �� ������ ����. �̰��� �ڵ鸵�Ͽ� �ø��� ����� �ϴµ�
*/

using namespace std;

SIORSerial::SIORSerial() {

}

SIORSerial::~SIORSerial() {
}

bool SIORSerial::OpenPort(CString portname) {
	m_hComm = CreateFile(L"//./" + portname, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0); //�ø��� ��Ʈ�� �����Ѵ�. 
	if (m_hComm == INVALID_HANDLE_VALUE)  //���������� ��Ʈ�� ���ȴ��� Ȯ��
	{
		return false;  //������ �ʾ��� ��� false ��ȯ
	}
	else
		return true;   //����� ������ ��� true ��ȯ
}

void SIORSerial::ClosePort() {
	CloseHandle(m_hComm); //��Ʈ�� �ݴ´�.
}


bool SIORSerial::ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity, BYTE Parity, BYTE StopBits) {
	if ((m_bPortReady = GetCommState(m_hComm, &m_dcb)) == 0) { //��Ʈ�� ���¸� Ȯ��. ���������� ������ �ʾ��� ��� false ��ȯ
		printf("\nGetCommState Error\n");
		//"MessageBox(L, L"Error", MB_OK + MB_ICONERROR);  
		CloseHandle(m_hComm);
		return false;
	}

	//	��Ʈ�� ���� �⺻���� ����
	//	�ڼ��Ѱ� http://purestarman.tistory.com/169 �� �����ִ�.

	m_dcb.BaudRate = BaudRate;		//	���巹��Ʈ
	m_dcb.ByteSize = ByteSize;		//	�������� �� ������ ��Ʈ��(4~8, ������ 7~8)
	m_dcb.Parity = Parity;			/*	����� �и�Ƽ
									EVENPARITY	: ¦�� �и�Ƽ
									MARKPARITY	: ��ũ �и�Ƽ
									NOPARITY	: ����
									ODDPARITY	: Ȧ�� �и�Ƽ
									SPACEPARITY	: ���� �и�Ƽ*/
	m_dcb.StopBits = StopBits;		/*	���� STOP ��Ʈ�� ���� �����Ѵ�.
									ONESTOPBIT		: 1��Ʈ
									ONE5STOPBITS	: 1.5��Ʈ
									TWOSTOPBITS		: 2��Ʈ*/
	m_dcb.fBinary = true;			//	���̳ʸ� ���� �� ���ΰ�? �׳� true �� �ϱ�
	m_dcb.fDsrSensitivity = false;	//	DSR ��ȣ �ö����� ��� ����Ʈ�� �������� ����
	m_dcb.fParity = fParity;		//	�и�Ƽ üũ Ȱ��ȭ ����
	m_dcb.fOutX = false;			/*	������ �� ���� XON/XOFF �帧 ���� ��� ����
									XonChar �� ������ �۽�, XoffChar ������ �۽� ����
									*/
	m_dcb.fInX = false;				//	������ �� ���� XON/XOFF �帧 ��� ������� ����
									//	m_dcb.XonChar = 'a';
									//	m_dcb.XoffChar = 'z';

	m_dcb.fNull = false;			//	NULL ����Ʈ�� �������� ����
	m_dcb.fAbortOnError = true;		//	�����߻��� R/W ���� ����
	m_dcb.fOutxCtsFlow = false;		//	����� CTS ��ȣ�� �������� ����
	m_dcb.fOutxDsrFlow = false;		//	����� DSR �� �������� ����
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxCtsFlow = false;

	m_bPortReady = SetCommState(m_hComm, &m_dcb);  //��Ʈ ���� Ȯ��

	if (m_bPortReady == 0) { //��Ʈ�� ���¸� Ȯ��. ������ ��� true ��ȯ �ƴ� ��� false ��ȯ
							 //MessageBox(L"SetCommState Error");  
		printf("SetCommState Error");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool SIORSerial::SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
	DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
	DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant) {//��� ��Ʈ�� ���� Timeout ����
	if ((m_bPortReady = GetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0)
		return false;

	m_CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout; //����Ҷ� �ѹ���Ʈ�� ���� �� ���� ����Ʈ�� ���۵ɶ������� �ð�
															  //��ſ��� �����͸� ���� �� Timeout�� ����� �������� ���� ����
	m_CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	//��ſ��� �����͸� ������ �� Timeout�� ����� �������� ���� ����
	m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

	m_bPortReady = SetCommTimeouts(m_hComm, &m_CommTimeouts);  //��Ʈ ���� Ȯ��

	if (m_bPortReady == 0) {	//��Ʈ ���°� ���� ���� ��� false��ȯ. �ƴ� ��� true��ȯ
								//MessageBox(L"StCommTimeouts function failed",L"Com Port Error",MB_OK+MB_ICONERROR);  
		printf("\nStCommTimeouts function failed\n");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool SIORSerial::WriteByte(char bybyte) {
	m_iBytesWritten = 0;

	if (WriteFile(m_hComm, &bybyte, 1, &m_iBytesWritten, NULL) == 0) //�Է¹��� ���� WriteFile�� ���� ��Ʈ�� �����Ѵ�.
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
	if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {//��Ʈ�� �����ϴ� �����͸� ReadFile�� ���� 1����Ʈ�� �о�´�.

		if (dwBytesTransferred == 1) {//�����͸� �о���µ� �������� ���
			resp = rx;  //resp�� �����͸� �����ϰ� true ��ȯ
			return true;
		}
	}

	return false; //�������� ��� false ��ȯ
}

bool SIORSerial::ReadByte(int &resp) {
	BYTE rx;
	resp = 0;

	DWORD dwBytesTransferred = 0;
	if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0)) {//��Ʈ�� �����ϴ� �����͸� ReadFile�� ���� 1����Ʈ�� �о�´�.

		if (dwBytesTransferred == 1) {//�����͸� �о���µ� �������� ���
			resp = rx;  //resp�� �����͸� �����ϰ� true ��ȯ
			return true;
		}
	}

	return false; //�������� ��� false ��ȯ
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