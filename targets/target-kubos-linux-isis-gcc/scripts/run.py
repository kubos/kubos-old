#!/usr/bin/python

import serial
import os

def send_read_line(ser, cmd):
    ser.write(cmd + "\n")
    return ser.readline()

def wait_on_string(ser, string):
    timeout = 10
    for i in range(1, timeout):
        lines = ser.readlines()
        for l in lines:
            if string in l:
                return True
    return False

def login(ser):
    send_read_line(ser, "")
    if "#" in send_read_line(ser, ""):
        print "logged in"
        return True
    print "logging in..."
    ser.write("\n")
    if wait_on_string(ser, "login:"):
        ser.write("root\r\n")
        if wait_on_string(ser, "Password:"):
            ser.write("Kubos123\r\n")
            return wait_on_string(ser, "#")
    return False

ser = serial.Serial('/dev/FTDI', 115200, timeout=1)

if login(ser):
    print "Sending app"
    ser.write("cd /home/system/usr/bin\n")
    ser.write("rm -f supervisor-client\n")
    ser.write("rz -w 8192\n")
    os.system("stty -F /dev/FTDI 115200")
    os.system("sz -y -w 8192 source/supervisor-client > /dev/FTDI < /dev/FTDI")
    ser.write("\n")
    print "Running app"
    ser.write("/home/system/usr/bin/supervisor-client\n")
    for l in ser.readlines():
        print l.strip()
    ser.write("exit\n")
else:
    print "Unable to login"
