#!/usr/bin/python

import serial
import os
import sys
import json

SERIAL_DEV = "/dev/FTDI"
SERIAL_SPEED = 115200

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

def send_file(ser, local_file, remote_folder):
    ser.write("cd %s\n" % remote_folder)
    ser.write("rz -w 8192\n")
    os.system("stty -F %s %s" % (SERIAL_DEV, SERIAL_SPEED))
    os.system("sz -y -w 8192 %s > %s < %s" % (local_file, SERIAL_DEV, SERIAL_DEV))

def login(ser, user, password):
    send_read_line(ser, "")
    if "#" in send_read_line(ser, ""):
        print "logged in"
        return True
    print "logging in..."
    ser.write("\n")
    if wait_on_string(ser, "login:"):
        ser.write("%s\r\n" % user)
        if wait_on_string(ser, "Password:"):
            ser.write("%s\r\n" % password)
            return wait_on_string(ser, "#")
    return False


def run_show_output(remote_file):
    blank_lines = 0
    line = ""

    ser.write("%s\n" % remote_file)

    while blank_lines < 3:
        line = ser.readline().strip()
        if line == "":
            blank_lines = blank_lines + 1
        else:
            blank_lines = 0
            print line

password = ""
dest_dir = ""
init_boot = ""
run_level = ""
app_name = ""

with open('yotta_config.json', 'r') as f:
    j = json.load(f)
    password = j.get("system").get("password").encode('ascii')
    dest_dir = j.get("system").get("destDir").encode('ascii')
    init_boot = j.get("system").get("initAtBoot")
    run_level = j.get("system").get("runLevel")
    init_flash = j.get("system").get("initAfterFlash")
    app_name = j.get("name", "exec")

app_name = "supervisor-client"

init_scripts = "%s%s" % (run_level, app_name)

ser = serial.Serial('/dev/FTDI', 115200, timeout=1)

if login(ser, "root", password):
    print "Sending app"
    send_file(ser, "source/supervisor-client", dest_dir)
    ser.write("\n")
    
    print "Running app"
    run_show_output("%s/%s" % (dest_dir, app_name))
    ser.write("exit\n")
else:
    print "Unable to login"
