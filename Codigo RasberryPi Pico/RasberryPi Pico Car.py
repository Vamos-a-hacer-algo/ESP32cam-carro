from machine import Pin,UART
import time

uart = UART (1, baudrate=115200, tx=Pin(4), rx=Pin(5))
uart.init(bits=8, parity=None, stop=2)

led=Pin("LED", Pin.OUT)
buzzer=Pin(2, Pin.OUT)
MR1=Pin(11,Pin.OUT) 
MR2=Pin(10, Pin.OUT)
ML1=(13,Pin.OUT)
ML2=Pin(12,Pin.OUT)

modo=0

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
        if data=='F':
            MR1.value(1)
            MR2.value(0)
            ML1.value(1)
            ML2.value(0)
        if data=='G':
            MR1.value(0)
            MR2.value(1)
            ML1.value(0)
            ML2.value(1)
        if data=='L':
            MR1.value(1)
            MR2.value(0)
            ML1.value(0)
            ML2.value(1)
        if data=='R':
            MR1.value(0)
            MR2.value(1)
            ML1.value(1)
            ML2.value(0)
        if data=='Q':
            MR1.value(1)
            MR2.value(0)
            ML1.value(0)
            ML2.value(0)
        if data=='E':
            MR1.value(0)
            MR2.value(0)
            ML1.value(1)
            ML2.value(0)
        if data=='Z':
            MR1.value(0)
            MR2.value(1)
            ML1.value(0)
            ML2.value(0)
        if data=='C':
            MR1.value(0)
            MR2.value(0)
            ML1.value(0)
            ML2.value(1)
        if data=='S':
            MR1.value(0)
            MR2.value(0)
            ML1.value(0)
            ML2.value(0)
    
            
    time.sleep(0.01)