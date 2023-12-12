from machine import Pin,UART,PWM
import time

uart = UART (1, baudrate=115200, tx=Pin(4), rx=Pin(5))
uart.init(bits=7, parity=None, stop=2)

led=Pin("LED", Pin.OUT)
buzzer=Pin(2, Pin.OUT)
MR1=Pin(11,Pin.OUT) 
MR2=Pin(10, Pin.OUT)
ML1=Pin(12,Pin.OUT)
ML2=Pin(13,Pin.OUT)
MOTOR= Pin(15, Pin.OUT)
pwm = machine.PWM(MOTOR)
pwm.freq(50)

modo=0

while True:
    if uart.any():
        data = uart.read()
        print (data)
        if data==b'M':
            led.toggle();
        if data==b'X':
            buzzer.toggle();
        if data==b'F':
            MOTOR.low()
            pwm.duty_u16(20574)
            MR1.value(1);
            MR2.value(0);
            ML1.value(1);
            ML2.value(0);
        if data==b'G':
            MOTOR.low()
            pwm.duty_u16(20574)
            MR1.value(0);
            MR2.value(1);
            ML1.value(0);
            ML2.value(1);
        if data==b'L':
            MOTOR.low()
            pwm.duty_u16(49150)
            MR1.value(1);
            MR2.value(0);
            ML1.value(0);
            ML2.value(1);
        if data==b'R':
            MOTOR.low()
            pwm.duty_u16(49150)
            MR1.value(0);
            MR2.value(1);
            ML1.value(1);
            ML2.value(0);
        if data==b'Q':
            MOTOR.low()
            pwm.duty_u16(49150)
            MR1.value(1);
            MR2.value(0);
            ML1.value(0);
            ML2.value(0);
        if data==b'E':
            MOTOR.low()
            pwm.duty_u16(49150)
            MR1.value(0);
            MR2.value(0)
            ML1.value(1)
            ML2.value(0)
        if data==b'Z':
            MOTOR.low()
            pwm.duty_u16(49150)
            MR1.value(0)
            MR2.value(1)
            ML1.value(0)
            ML2.value(0)
        if data==b'C':
            MOTOR.low()
            pwm.duty_u16(49150)
            MR1.value(0)
            MR2.value(0)
            ML1.value(0)
            ML2.value(1)
        if data==b'S':
            MR1.value(0)
            MR2.value(0)
            ML1.value(0)
            ML2.value(0)
            
    time.sleep(0.01)