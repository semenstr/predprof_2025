import serial
from openpyxl import *
import datetime
if __name__ == '__main__':
    fio = ""

    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flush()
    while True:
        wb = load_workbook('/home/pi/Desktop/Python/base.xlsx')
        ws = wb.active
        flag = 0
        if ser.in_waiting > 0:
            flag = 0
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
            for row in ws.values:
                r = list(row)
                if line == r[1]:
                    if datetime.datetime.now() <=  r[2]:
                        fio = r[0]
                        fio = bytes(f"{fio}\n", encoding = 'utf-8')
                        flag = 1
                        break

            if flag == 1:
                ser.write(b"Access allowed\n")
                ser.write(fio)
                print("acces aloved")
                flag = 0
            else:
                ser.write(b"Access denied\n")
                print("acces net aloved")
                flag = 0
