import serial
import random
import math
import time

# Fixed Value
G_PORT = "COM6"
G_BAUD_RATE = 115200

G_FILE_NAME = "g_input.txt"
G_FILE_NAMES = "g_input.txt"
G_MAX_SPEED = 128       # min : 0, mid or zero_speed : 63, max : 127, 아두이노에서 변환한다.
G_LINE_ANGLE = 3600     # min : -1700, max : 1700, 민맥의 차 이다.
G_SERIAL = serial.Serial(G_PORT, G_BAUD_RATE)
G_NEXT_STATE = []   # G_NEXT_STATE[state = angle][action]

# Variable
# G_NEXT_STATE[G_PREVIOUS_STATE][G_PREVIOUS_ACTION] = G_PRESENT_STATE 를 더한다.
G_BLOCK_ANGLE = 50       # 블록당 크기, 이게 작을수록 정밀도랑 배열의 크기가 늘어난다.
G_SIZE_OF_SPEED = int(G_MAX_SPEED) + 1  # 배열의 크기를 결정한다. G_NEXT_STATE[state][action] 에서 action 의 크기다.
G_SIZE_OF_ANGLE = int(G_LINE_ANGLE / G_BLOCK_ANGLE) + 1  # 각도 블럭의 크기는 잘 조정해본다.
G_PREVIOUS_STATE = 300
G_PRESENT_STATE = 300
G_PRESENT_ACTION = 999
G_PREVIOUS_ACTION = 999
G_STANDARD_DEVIATION = 8        # For generate Gaussian Random Variable
G_ACCUMULATE_DEV = 100


class STATEMENT:
    def __init__(self):
        self.sum_of_statement = 0
        self.cnt_of_statement = 0

    def add_data(self, _sum):
        self.sum_of_statement += _sum
        self.cnt_of_statement += 1

    # 작은 값을 뱉어낼수록 좋은것이다.
    # 아무런 값도 학습되어있지 않으면 가장 좋은 값을 뱉어준다.
    # 그래야 모든 action 을 다 돌아볼 수 있다.
    # G_LINE_ANGLE / (G_BLOCK_ANGLE * 2)  일때가 수평이다.
    def return_score(self):
        if self.cnt_of_statement > 0:
            _average = self.sum_of_statement / self.cnt_of_statement
            _ret_value = abs(_average - (G_LINE_ANGLE / (G_BLOCK_ANGLE * 2)))
        else:
            _ret_value = -1
    #    print("        return : " + str(_ret_value))
        return _ret_value


# METHOD AREA
def write_file():
    count_zero = 0
    g_file = open(G_FILE_NAMES, 'w')
    for i1 in range(0, G_SIZE_OF_ANGLE):
        for j1 in range(0, G_SIZE_OF_SPEED):
            tmp_data = "%d " % int(G_NEXT_STATE[i1][j1].cnt_of_statement)
            if int(tmp_data) == 0:
                count_zero += 1
            tmp_data2 = "%d " % int(G_NEXT_STATE[i1][j1].sum_of_statement)
            if int(tmp_data2) == 0:
                count_zero += 1
            data = tmp_data + tmp_data2
            g_file.write(data)
        g_file.write('\n')
    g_file.write(str(count_zero))
    g_file.close()


def read_file():
    gg_file = open(G_FILE_NAME, 'r')
    for i2 in range(0, G_SIZE_OF_ANGLE):
        tmp_input1 = gg_file.readline()
        tmp_arr1 = tmp_input1.split(' ')
        cnt = 0
        for j2 in range(0, G_SIZE_OF_SPEED):
            G_NEXT_STATE[i2][j2].cnt_of_statement = int(tmp_arr1[cnt])
            cnt += 1
            G_NEXT_STATE[i2][j2].sum_of_statement = int(tmp_arr1[cnt])
            cnt += 1
    gg_file.close()


def init_matrix():

    for i3 in range(0, G_SIZE_OF_ANGLE):
        tmp_arr2 = []
        for j3 in range(0, G_SIZE_OF_SPEED):
            tmp_state2 = STATEMENT()
            tmp_arr2.append(tmp_state2)
        G_NEXT_STATE.append(tmp_arr2)


# Start Main
for i in range(0, G_SIZE_OF_ANGLE):
    tmp_arr = []
    for j in range(0, G_SIZE_OF_SPEED):
        tmp_state = STATEMENT()
        tmp_arr.append(tmp_state)
    G_NEXT_STATE.append(tmp_arr)

is_more = 0
read_file()
cnt_loop = int(0)

g_time_bef = time.time()
g_time_aft = time.time()

# 이 조건을 시리얼 통신이 가능할때로 바꿔준다.
print("Beginning")
while G_SERIAL.is_open:

    received_signal = " "
    received_signal = G_SERIAL.readline()
    received_signal = received_signal.decode()
    print(received_signal)

    sensor_value = int(received_signal)

    if received_signal == "END\r\n":
        G_NEXT_STATE[G_PREVIOUS_STATE][G_PREVIOUS_ACTION].add_data(129)
        G_PRESENT_STATE = G_PREVIOUS_STATE

    else:
        sensor_value = int(sensor_value) + 1800
        G_PRESENT_STATE = int(sensor_value / G_BLOCK_ANGLE)
        if cnt_loop > 0:
            G_NEXT_STATE[G_PREVIOUS_STATE][G_PREVIOUS_ACTION].add_data(G_PRESENT_STATE)

    best_value = 999
    best_action = 63

    #   temp_gauss 변수를 이용해서 어느정도 랜덤한 선택을 할 수 있도록 한다.
    #   temp_gauss = 가우스변수 * log(학습수) / 학습수  를 적용하면
    #   학습이 진행될수록 가우스변수에 의한 영향을 덜 받게 된다.
    for i in range(0, G_SIZE_OF_SPEED):
        temp_cnt = G_NEXT_STATE[G_PRESENT_STATE][i].cnt_of_statement + 1
        temp_gauss = random.gauss(0, G_STANDARD_DEVIATION + temp_cnt)

        temp_cnt = math.log2(temp_cnt + 1) / temp_cnt
        final_gauss = temp_gauss * temp_cnt + G_NEXT_STATE[G_PRESENT_STATE][i].return_score()

        if best_value > final_gauss:
            best_value = final_gauss
            best_action = i

    G_SERIAL.write(str(chr(best_action)).encode())

    G_PREVIOUS_STATE = G_PRESENT_STATE
    G_PREVIOUS_ACTION = best_action

    if cnt_loop % 100 == 0:
        write_file()
        g_time_aft = time.time()
        print(str(cnt_loop) + " counts, " + str(round(g_time_aft - g_time_bef, 3)) + " seconds")
        g_time_bef = g_time_aft

    cnt_loop = cnt_loop + 1

print("End Transmission : " + str(cnt_loop))
write_file()