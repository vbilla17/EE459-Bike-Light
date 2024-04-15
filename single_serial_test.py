import serial
import time

# Open the serial port
ser = serial.Serial('/dev/serial0', baudrate=9600, timeout=1)

try:
    print("Sending data...")
    # Send a test string
    test_string = "Hello UART!".encode('utf-8')
    ser.write(test_string)
    
    time.sleep(1)  # Give some time for data to loop back
    
    # Read the data
    if ser.in_waiting > 0:
        incoming_data = ser.read(ser.in_waiting)
        print("Received data:", incoming_data)
        
        # Check if the sent and received data are the same
        if incoming_data == test_string:
            print("Loopback successful. UART is working.")
        else:
            print("Data received is different from what was sent. Check setup.")
    else:
        print("No data received. Check connections and settings.")
finally:
    ser.close()  # Close the serial port to clean up
