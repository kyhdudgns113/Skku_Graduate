/*
*		Copyleft by Young-Hoon Kang in Sungkyunkwan University
*		�ø��� ����� Python ���� ��������� �ӵ��� �ſ� ���� C++ �� ����ϱ� ���� �������.
*		���̽��̳� �Ƶ��̳뿡�� �ø��� ����� �ϴ°Ͱ� ����ϰ� �ϱ� ���ؼ�
*		����� ������ �Ȱ���, �̸��� �ٸ� �Լ����� ������ �������.
*
*		���� ���ǻ���
*
*		�ø��� ����� �� ��, �����͸� �ùٸ��� ������ ������Ѵ�.
*		���� Serial.print(arg) �Լ��� ���� arg �� ���� char[] �÷� �ٲپ ������ �Ѵ�.
*		������ ���� NULL ���ڸ� �ִ� ���ڿ��� ���·� ������ �ϴ°��� �ƴϹǷ�
*		SIORSerial.ReadString() �Լ��� ���ؼ� �о���̸� ����� ���������� �������̴�.
*
*		���� ���� 2018-10-24
*/

#include <Windows.h>
#include <atlstr.h>
#include <string>

#define BUFFER_SIZE 128

class SIORSerial {
public:
	SIORSerial(void);
	virtual ~SIORSerial(void);

private:
	HANDLE  m_hComm;
	DCB     m_dcb;
	COMMTIMEOUTS m_CommTimeouts;
	bool    m_bPortReady;
	bool    m_bWriteRC;
	bool    m_bReadRC;
	DWORD   m_iBytesWritten;
	DWORD   m_iBytesRead;
	DWORD   m_dwBytesRead;

public:
	//	portname(ex : COM6) ��Ʈ�� ���� �Լ��̴�.
	//	��Ʈ�� ���� ��ɸ� �����ϱ� ������ ���巹��Ʈ, ������ ����
	//	ConfigurePort �Լ��� ���� ������ ������Ѵ�.
	bool OpenPort(CString portname);
	void ClosePort();

	bool ReadByte(BYTE &resp);

	//	int �� ������ ������, ��� 1����Ʈ�� �޾ƿ´�.
	//	4����Ʈ�� �ޱ� ���ؼ���
	//		1. ù ��° ���ڿ� ������ �ּҰ��� �ִ´�.
	//		2, �о�� ����Ʈ�� �־��ش�.
	bool ReadByte(int &resp);
	bool ReadByte(BYTE* &resp, UINT size);

	//	Input signal must be ended by '\n'.
	//	By using 'Serial.println' function or adding '\n' or '\0' in end of string
	//	Maximum string size is BUFFER_SIZE which is already defined
	//	\0 Ȥ�� NULL ����(int 0) �� �ȹ޵��� �Ͽ���.
	bool ReadString(std::string &dest);
	//	Input signal must be ended by '\n'.
	//	By using 'Serial.println' function or adding '\n' or '\0' in end of string
	//	Maximum string size is BUFFER_SIZE which is already defined
	//	\0 Ȥ�� NULL ����(int 0) �� �ȹ޵��� �Ͽ���.
	bool ReadString(char *dest);

	bool WriteByte(char bybyte);
	bool WriteString(std::string &src);
	bool WriteString(char *src);

	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
		DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
		DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant);
	bool ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity,
		BYTE  Parity, BYTE StopBits);

	//	python �� Serial �Լ��� �ִ��� ������ �Ѱ��̴�.
	//	portName		: ��Ʈ�� �̸�, ���� �����ؾ���.
	//	baudRate		: ���巹��Ʈ, ���� �����ؾ���.
	//	byteSize = 8	: �� ���� �ְ���� ��Ʈ ��, 1����Ʈ�� �ϱ����� 8�� �⺻�̴�. 
	//	fParity = 0		: �и�Ƽ üũ�� �� �� ����, false �� �ʱ�ȭ��
	//	Parity = 0		: ���� �и�Ƽ�� ������ �����Ѵ�. 
	//						EVENPARITY : ¦��
	//						MARKPARITY : ��ũ �и�Ƽ
	//						NOPARITY = 0 : ����
	//						ODDPARITY : Ȧ�� �и�Ƽ
	//						SPACEPARITY : ����
	//	StopBits = 0	: stop ��Ʈ�� ���� �����Ѵ�.
	//						ONESTOPBIT = 0 : 1��Ʈ
	//						ONESTOPBITS = 1 : 1.5��Ʈ
	//						TWOSTOPBITS = 2 : 2��Ʈ	
	//	begin �Լ��� ����� ������ �����ϴ�.
	bool Serial(std::string portName, DWORD baudRate, BYTE byteSize = 8,
		DWORD fParity = false, BYTE Parity = NOPARITY, BYTE stopBits = TWOSTOPBITS);

	//	python �� Serial �Լ��� �ִ��� ������ �Ѱ��̴�.
	//	portName �� baudRate �� ������ ������ ������ϸ�
	//	byteSize = 8 �� ������ ������ ������ �̼����� �ڵ�����
	//	0���� �ʱ�ȭ�ȴ�.
	//	Serial �Լ��� ����� ������ �����ϴ�.
	bool begin(std::string portName, DWORD baudRate, BYTE byteSize = 8,
		DWORD fParity = false, BYTE Parity = NOPARITY, BYTE stopBits = ONESTOPBIT);

	bool isOpen();

	void terminate();
	void flush();
};