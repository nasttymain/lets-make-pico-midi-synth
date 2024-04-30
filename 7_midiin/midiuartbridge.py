import mido
import sys
import serial
import serial.tools.list_ports

# MIDI
devname: str = ""
if len(mido.get_input_names()) == 0:
    print("No MIDI Input. gg")
    sys.exit(1)
if len(mido.get_input_names()) == 1:
    devname = mido.get_input_names()[0]
if len(mido.get_input_names()) >= 2:
    while(True):
        print("Which:")
        print("\n".join([str(str(_[0]) + ": " + _[1]) for _ in enumerate(mido.get_input_names())]))
        s = input()
        if s.isnumeric():
            if int(s) in range(len(mido.get_input_names())):
                sn: int = int(s)
                devname = mido.get_input_names()[sn]
                break
        print("Bro ", end = "")
try:
    inport = mido.open_input(name=devname)
except Exception as e:
    print("MIDI not good :/ ", e)
    sys.exit(2)

print("Opened " + devname)

# Serial
coms = ([_.device for _ in serial.tools.list_ports.comports()])
comname: str = ""
if len(coms) == 0:
    print("No COM. bye")
    sys.exit(3)
if len(coms) == 1:
    comname = coms[0]
if len(coms) == 2:
    while(True):
        print("Which:")
        print("\n".join([str(str(_[0]) + ": " + _[1]) for _ in enumerate(coms)]))
        s = input()
        if s.isnumeric():
            if int(s) in range(coms):
                sn: int = int(s)
                comname = coms[sn]
                break
        print("Bro ", end = "")
try:
    outser = serial.Serial(port = comname, baudrate = 115200, bytesize = 8, parity = "N", stopbits = 1)
except Exception as e:
    print("BRUH your serial is ", e)
    sys.exit(4)

print("Opened " + comname)



for msg in inport:
    if(msg.bytes()[0] == 248):
        # Ignoring Master Sync
        # print("sync ", msg.bytes())
        outser.write(msg.bytes())
        pass
    else:
        print(msg.bytes())
        outser.write(msg.bytes())