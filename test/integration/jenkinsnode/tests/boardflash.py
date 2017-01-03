#!/usr/bin/python

# Kubos Continuous Integration
# Copyright (C) 2016 Kubos Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
# Depending on your udev rules (see /etc/udev/rules.d/ ) you may have to
# run this script as root
#

# Should execute this script with three command line arguments:
# -f compiled binary file to upload, including path if needed
# -b typeofboard ( that is, stm32f407-disco-gcc, pyboard-gcc, or msp430f5529-gcc ) 
#


import sys
import os
from time import sleep
import RPi.GPIO as GPIO
import cistack as ci
import subprocess

kwargs = ci.readOpts()

print(str(kwargs))

# 'inputbinary': 'kubos-rt-example', 'binfilepath': '/home/kubos', 'verbose': True, 'board': 'stm32f407-disco-gcc'
binfile = kwargs['inputbinary']
binpath = kwargs['binfilepath']
board = kwargs['board']

target = ci.getTarget(board)

freepinswhendone = 0
shutdownwhendone = 0
ignoreGPIOwarnings = False

if (ignoreGPIOwarnings):
    GPIO.setwarnings(False)


target.setupboard()
print ("Board setup complete.")
sleep(1)

target.powerup()
print("Powering on the board")
sleep(1)

target.progmode()
sleep(1)

if (target.flash(binfile, binpath) is True):
    print("Program flash completed. Reports success.")
else:
    print("Program may have failed. Exiting")
    sys.exit()

# program returns 0 here so Jenkins can see success

#else:
#    print("*** ERROR: Program flash appears to have failed.")
#    sys.exit("Halting script")


target.reset()
print("\nBoard reset.")

sleep(1)

if(shutdownwhendone):
    print("Shutting down the board.")
    target.powerdown() 

# If you want to shut the board down, this command cleans up and 
# de-energizes the power MOSFET.
if(freepinswhendone):
    print("Freeing pins and exiting.") 
    ci.allDone()



#<EOF>
