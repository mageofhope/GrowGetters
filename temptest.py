import machine
import time

thermometer = machine.ADC(4)

while(True):
    aRead = thermometer.read_u16()
    vRead = aRead * 3.266 / 65535
    tRead = 27 - (vRead - 0.706)/0.001721
    print(aRead)
    print(vRead)
    print(tRead)
    time.sleep(1)