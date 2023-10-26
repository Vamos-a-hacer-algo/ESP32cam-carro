from machine import Pin,UART
import time

uart = UART (1, baudrate=115200, tx=Pin(4), rx=Pin(5))
uart.init(bits=8, parity=None, stop=2)

led=Pin("LED", Pin.OUT)
buzzer=Pin(2, Pin.OUT)

while True:
    if uart.any():
        data = uart.read()
        print (data)
        if data==b'M\n':
            led.toggle();
        if data==b'X\n':
            buzzer.value(1);
        if data==b'X':
            buzzer.value(0);
    time.sleep(0.01)