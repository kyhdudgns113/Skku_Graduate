1. Beta_Test

	하드웨어가 잘 작동하는지 테스트한 코드이다.
	
	1. Check_Motor		: 모터가 잘 작동하는지 테스트한 코드이다.
	2. Check_Sensor		: 센서에서 값을 잘 읽는지 확인한 코드이다.
	3. Check_Combination	: 센서와 모터가 잘 연동되는지 확인한 코드이다.


2. FirstModel

	파이썬이랑 통신을 하며 학습을 하는 첫 번째 모델이다.

	1. ARDUINO_MOTOR	: 아두이노 파트의 메인 코드이다.
	2. GKD_MPU6050		: mpu6050 센서에 칼만 필터 코드를 적용한 라이브러리이다.

3. CPP_Test_Model

	파이썬에서 cpp 로 바꾸면서 작동이 잘 되는지 테스트를 하기 위한 코드이다.
	next angle = previous_angle + 3 * present_speed + previous_speed
	라는 간단한 모델로 테스트 모델을 학습시켰다.

	1. Second_Test_Arduino	: 아두이노부에서의 작동코드, 위에서 설정한 가상의 환경을 가동함. 어떠한 신호가 왔는지 LED 로 확인함
	2. Second_Test_Model	: PC 부의 작동코드, Cpp 로 작성됨. 데이터를 받는 과정에서의 노이즈는 없다고 가정함.
	3. SIORSerial		: Cpp 로 시리얼통신을 하기 위한 클래스, 오픈소스에서 발생하는 버그들을 상당 수정하고 이용의 편의성을 개선함.


