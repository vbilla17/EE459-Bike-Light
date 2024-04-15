import serial
import time

# Open the serial port. Adjust '/dev/ttyAMA0' to your Raspberry Pi's serial port if different.
ser = serial.Serial('/dev/serial0', baudrate=9600, timeout=1)

try:
    rv = ""
    while True:
        if ser.in_waiting > 0:
            # incoming_data = ser.read(ser.in_waiting)  # Read all available data
            # print(f"Received: {incoming_data}")
            ch = ser.read()
            if ch.decode("utf-8") == 'L':
                while ch.decode("utf-8") != '\n':
                    rv += ch.decode("utf-8")
                    ch = ser.read()
                print(rv)
                rv = ""
        else:
            print("No data.")
        time.sleep(1.0)  # Sleep for a bit to avoid spamming the terminal
        # test_string = "Hello UART!".encode('utf-8')
        # ser.write(test_string)
except KeyboardInterrupt:
    print("Program terminated by user")
finally:
    ser.close()  # Ensure the serial connection is closed on program exit
