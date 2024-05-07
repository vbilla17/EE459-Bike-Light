import tkinter
import tkintermapview
from compass import CompassWidget
from threading import Thread
import time
import serial
import os
import geopy.distance

global lat, lon, prev_lat, prev_lon, speed, heading, btn1_state, prev_btn1_state, btn2_state, prev_btn2_state, elapsed_time, distance, stopwatch_state
# lat, lon = 34.02171, -118.28890
lat, lon = 0, 0
speed = 0
heading = 0
# time_status_bit = 0
btn1_state = 1
prev_btn1_state = 1
btn2_state = 1
prev_btn2_state = 1
elapsed_time = 0
distance = 0
stopwatch_state = 'r' # 'r' for reset, 'c' for counting, 'x' for stopped

ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)


def read_sentence():
    message = ''
    ser.reset_input_buffer()  # Clear the input buffer
    time.sleep(0.1)
    while True:
        if ser.in_waiting > 0:
            char = ser.read(1).decode() 
            
            # Start of a new message
            if char == '$':  
                message = char
            elif char == '\n' and message.startswith('$'):
                message += char
                
                # Return the complete message
                return message[1:-1]  
            elif message.startswith('$'):
                message += char
        else:
            time.sleep(0.001) 
    


def parse_sentence(sentence):
    global lat, lon, prev_lat, prev_lon, speed, heading, time_status_bit, btn1_state, prev_btn1_state, btn2_state, prev_btn2_state, distance
    # sentence format:

    # $<gps_valid><lat>,<lat_dir>,<lon>,<lon_dir>,<speed>,<heading>,<btn_1><btn_2>\n
    # $ddmm.mmmm,d,ddmm.mmmm,d,kk.kkk,dd.dd,s\n
    # Split the sentence by commas into components
    (
        raw_gps_valid,
        utc,
        raw_lat,
        raw_lat_dir,
        raw_lon,
        raw_lon_dir,
        raw_speed,
        raw_heading,
        raw_btn_1,
        raw_btn_2,
    ) = sentence.split(",")

    if int(raw_gps_valid) == 1:
        prev_lat = lat
        prev_lon = lon
        lat = convert_to_decimal(raw_lat, raw_lat_dir)
        print(f"lat: {lat}\nlon: {lon}\n")
        # print(f"lat_dir: {raw_lat_dir}\nlon_dir: {raw_lon_dir}\n")
        lon = convert_to_decimal(raw_lon, raw_lon_dir)

        heading = float(raw_heading)
        speed = int(float(raw_speed) * 1.15)  # convert from knots to mph

    prev_btn1_state = int(btn1_state)
    btn1_state = int(raw_btn_1)
    
    prev_btn2_state = int(btn2_state)
    btn2_state = int(raw_btn_2)
    
    travel_distance = geopy.distance.geodesic((lat, lon), (prev_lat, prev_lon)).miles
    if travel_distance < 10 and stopwatch_state == 'c':
        distance += travel_distance


# converts ddmm.mm (lat), dddmm.mm (lon) to decimal degrees
def convert_to_decimal(coord, direction):
    # Determine the number of digits for degrees based on direction
    # Latitude (N, S) always has two digits for degrees, Longitude (E, W) always has three
    degrees_digits = 2 if direction in ['N', 'S'] else 3

    # Extract degrees and minutes based on the determined split
    degrees = int(coord[:degrees_digits])
    minutes = float(coord[degrees_digits:])

    # Convert to decimal degrees
    decimal = degrees + minutes / 60

    if direction in ["S", "W"]:
        decimal = -decimal

    return decimal



def handle_data(root):
    global lat, lon, speed, heading, time_status_bit, elapsed_time, stopwatch_state, btn1_state, btn2_state, distance
    while True:
        if ser.in_waiting > 0:
            sentence = read_sentence()
            parse_sentence(sentence)
        else:

            time.sleep(1.0)
            continue

        # State transitions based on current state and button presses
        if stopwatch_state == 'r':
            # Move to 'c' state if btn1 is pressed
            if btn1_state != prev_btn1_state:
                stopwatch_state = 'c'
        elif stopwatch_state == 'c':
            # Move to 'x' state if btn2 is pressed
            if btn2_state != prev_btn2_state:
                stopwatch_state = 'x'
        
        elif stopwatch_state == 'x':
            # Move to 'r' state if btn2 is pressed again
            if btn2_state != prev_btn2_state:
                stopwatch_state = 'r'
                elapsed_time = 0
                distance = 0
                

        # root_tk.event_generate("<<panel_event>>")

        time.sleep(1.0) 
        
def move_timer_generator(root):
    global elapsed_time
    while(True):
        root_tk.event_generate("<<timer_event>>")
        time.sleep(1.0)
        if stopwatch_state is 'c':
            elapsed_time += 1
            


disp_width = 600
disp_height = 400

root_tk = tkinter.Tk()

# Frame for the map
frame1 = tkinter.Frame(root_tk, width=2 / 3 * disp_width, height=disp_height)
frame1.grid(row=0, column=0, sticky="nswe")
root_tk.grid_columnconfigure(0, weight=2)  

# Frame for the compass and labels
frame2 = tkinter.Frame(root_tk, width=1 / 3 * disp_width, height=disp_height, bg="black")
frame2.grid(row=0, column=1, sticky="nswe")
root_tk.grid_columnconfigure(1, weight=1)  
frame2.grid_propagate(False)  

# Define colors and fonts
text_color = "#39FF14"  
value_font = ("Verdana", 20)  
text_font = ("Verdana", 10)  

# Speed value and label
speed_value = tkinter.Label(frame2, text="0", font=value_font, fg=text_color, bg="black")
speed_value.grid(row=0, column=0, sticky="s", padx=30, pady=(10, 0))
speed_label = tkinter.Label(frame2, text="mph", font=text_font, fg=text_color, bg="black")
speed_label.grid(row=1, column=0, sticky="n", padx=30, pady=(0, 10))

# Compass widget
compass_widget = CompassWidget(frame2, width=100, height=100)  
compass_widget.grid(row=2, column=0, pady=(10, 10))

# Time value and label
time_value = tkinter.Label(frame2, text="00:00:00", font=value_font, fg=text_color, bg="black")
time_value.grid(row=3, column=0, sticky="s", padx=30, pady=(10, 0))
time_label = tkinter.Label(frame2, text="time", font=text_font, fg=text_color, bg="black")
time_label.grid(row=4, column=0, sticky="n", padx=30, pady=(0, 10))

# Distance value and label
distance_value = tkinter.Label(frame2, text="0.00", font=value_font, fg=text_color, bg="black")
distance_value.grid(row=5, column=0, sticky="s", padx=30, pady=(10, 0))
distance_label = tkinter.Label(frame2, text="miles", font=text_font, fg=text_color, bg="black")
distance_label.grid(row=6, column=0, sticky="n", padx=30, pady=(0, 10))

# path for the database to use
script_directory = os.path.dirname(os.path.abspath(__file__))
database_path = os.path.join(script_directory, "offline_tiles.db")

# Map widget
map_widget = tkintermapview.TkinterMapView(
    frame1,
    width=500,
    height=400,
    corner_radius=0,
    use_database_only=True,
    database_path=database_path,
)
map_widget.pack(fill="both", expand=True)
map_widget.set_zoom(18)

# Init poisition for map widget
map_widget.set_position(lat, lon, marker=False)

# Current location dot; always stays in the middle of the map while the map moves
marker_size = 10
marker_canvas = tkinter.Canvas(
    frame1, width=marker_size, height=marker_size, bg="white", highlightthickness=0
)
marker_canvas.create_oval(0, 0, marker_size, marker_size, fill="blue")
marker_canvas.place(relx=0.5, rely=0.5, anchor="center")

# def move():
#     global lat, lon, speed, heading, speed, elapsed_time
#     map_widget.set_position(lat, lon)
#     compass_widget.update_arrow_direction(heading, "E")
#     speed_value.config(text=f"{speed}")

#     hours = elapsed_time // 3600
#     remaining_seconds = elapsed_time % 3600
#     minutes = remaining_seconds // 60
#     seconds = remaining_seconds % 60

#     # Format the time into HH:MM:SS
#     time_value.config(text=f"{hours:02}:{minutes:02}:{seconds:02}")
#     distance_value.config(text=f"{distance:04.1f}")
#     root_tk.after(1000, move)


def move_map(event):
    global lat, lon, speed, heading, speed, elapsed_time
    map_widget.set_position(lat, lon)
    compass_widget.update_arrow_direction(heading, "E")
    speed_value.config(text=f"{speed}")

    hours = elapsed_time // 3600
    remaining_seconds = elapsed_time % 3600
    minutes = remaining_seconds // 60
    seconds = remaining_seconds % 60

    # Format the time into HH:MM:SS
    time_value.config(text=f"{hours:02}:{minutes:02}:{seconds:02}")
    distance_value.config(text=f"{distance:04.1f}")
    


# root_tk.bind("<<data_ready>>", move)
root_tk.bind("<<timer_event>>", move_map)
# root_tk.bind("<<panel_event>>", update_panel)

data_handler_thread = Thread(target=handle_data, args=(root_tk,))
data_handler_thread.daemon = True  # Makes sure the thread will close with the application
data_handler_thread.start()

timer_thread = Thread(target=move_timer_generator, args=(root_tk,))
timer_thread.daemon = True
timer_thread.start()

root_tk.mainloop()
