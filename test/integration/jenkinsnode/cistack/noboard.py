import os
import sys
import magic
import re
import subprocess
import logging
from time import sleep
import RPi.GPIO as GPIO
from utils import findBin, whichUSBboard, getBoardConfigs
from target import Target
from pin import Pin

class NoBoard(Target):
    def __init__(self):
        self.board = "noboard"
        self.arch = "noarch"
        self.cpu = "nocpu"
        self.binfiletype = "nobinfile"
        self.pins = {
            'rst' : Pin(name = 'rst', number = 17),
            'pwr' : Pin(name = 'pwr', number = 27),
            'opt' : Pin(name = 'opt', number = 22)
        }


