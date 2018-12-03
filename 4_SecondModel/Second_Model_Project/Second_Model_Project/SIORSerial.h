/*
*		Copyleft by Young-Hoon Kang in Sungkyunkwan University
*		시리얼 통신을 Python 보다 상대적으로 속도가 매우 빠른 C++ 로 사용하기 위해 만들었다.
*		파이썬이나 아두이노에서 시리얼 통신을 하는것과 비슷하게 하기 위해서
*		기능이 완전히 똑같고, 이름만 다른 함수들을 여러개 만들었다.
*
*		사용시 유의사항
*
*		시리얼 통신을 할 때, 데이터를 올바르게 전송을 해줘야한다.
*		가령 Serial.print(arg) 함수를 쓰면 arg 의 값을 char[] 꼴로 바꾸어서 전송을 한다.
*		하지만 끝에 NULL 문자를 넣는 문자열의 형태로 전송을 하는것이 아니므로
*		SIORSerial.ReadString() 함수를 통해서 읽어들이면 제대로 읽혀지지가 않을것이다.
*
*		최종 수정 2018-10-24
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
	//	portname(ex : COM6) 포트를 여는 함수이다.
	//	포트를 여는 기능만 제공하기 때문에 보드레이트, 딜레이 등은
	//	ConfigurePort 함수로 따로 설정을 해줘야한다.
	bool OpenPort(CString portname);
	void ClosePort();

	bool ReadByte(BYTE &resp);

	//	int 형 변수에 받지만, 사실 1바이트만 받아온다.
	//	4바이트를 받기 위해서는
	//		1. 첫 번째 인자에 변수의 주소값을 넣는다.
	//		2, 읽어올 바이트를 넣어준다.
	bool ReadByte(int &resp);
	bool ReadByte(BYTE* &resp, UINT size);

	//	Input signal must be ended by '\n'.
	//	By using 'Serial.println' function or adding '\n' or '\0' in end of string
	//	Maximum string size is BUFFER_SIZE which is already defined
	//	\0 혹은 NULL 문자(int 0) 은 안받도록 하였다.
	bool ReadString(std::string &dest);
	//	Input signal must be ended by '\n'.
	//	By using 'Serial.println' function or adding '\n' or '\0' in end of string
	//	Maximum string size is BUFFER_SIZE which is already defined
	//	\0 혹은 NULL 문자(int 0) 은 안받도록 하였다.
	bool ReadString(char *dest);

	bool WriteByte(char bybyte);
	bool WriteString(std::string &src);
	bool WriteString(char *src);

	bool SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
		DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
		DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant);
	bool ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity,
		BYTE  Parity, BYTE StopBits);

	//	python 의 Serial 함수를 최대한 구현을 한것이다.
	//	portName		: 포트의 이름, 직접 설정해야함.
	//	baudRate		: 보드레이트, 직접 설정해야함.
	//	byteSize = 8	: 한 번에 주고받을 비트 수, 1바이트로 하기위해 8이 기본이다. 
	//	fParity = 0		: 패리티 체크를 할 지 여부, false 로 초기화됨
	//	Parity = 0		: 사용될 패리티의 종류를 지정한다. 
	//						EVENPARITY : 짝수
	//						MARKPARITY : 마크 패리티
	//						NOPARITY = 0 : 없음
	//						ODDPARITY : 홀수 패리티
	//						SPACEPARITY : 간격
	//	StopBits = 0	: stop 비트의 수를 설정한다.
	//						ONESTOPBIT = 0 : 1비트
	//						ONESTOPBITS = 1 : 1.5비트
	//						TWOSTOPBITS = 2 : 2비트	
	//	begin 함수랑 기능이 완전히 동일하다.
	bool Serial(std::string portName, DWORD baudRate, BYTE byteSize = 8,
		DWORD fParity = false, BYTE Parity = NOPARITY, BYTE stopBits = TWOSTOPBITS);

	//	python 의 Serial 함수를 최대한 구현을 한것이다.
	//	portName 과 baudRate 는 무조건 설정을 해줘야하며
	//	byteSize = 8 을 제외한 나머지 값들은 미설정시 자동으로
	//	0으로 초기화된다.
	//	Serial 함수랑 기능이 완전히 동일하다.
	bool begin(std::string portName, DWORD baudRate, BYTE byteSize = 8,
		DWORD fParity = false, BYTE Parity = NOPARITY, BYTE stopBits = ONESTOPBIT);

	bool isOpen();

	void terminate();
	void flush();
};