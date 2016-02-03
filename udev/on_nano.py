#!/usr/bin/python

import sys

# eg. '/devices/pci0000:00/0000:00:1d.0/usb2/2-1/2-1.1/2-1.1.3/2-1.1.3:1.0/ttyUSB0/tty/ttyUSB0'
#                                                              ^ use this part

s = sys.argv[1] # path
parts = s.split("/")

#device = parts[-1]
s = parts[-4]
s, _ = s.split(":")
for c in ".":
    s = s.replace(c, "_")

name = "nano_" + s
print name

# FIN
