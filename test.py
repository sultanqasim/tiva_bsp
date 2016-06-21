import serial
import sys

def main(port="/dev/tty.usbmodem0E21DC21"):
    ser = serial.Serial(port, 800000)
    ser.write(b'\xEF\xBE\xAD\xDE')
    ser.write(b'\x02\x0E')
    print(ser.read(5))

if __name__ == "__main__":
    main(*sys.argv[1:])
