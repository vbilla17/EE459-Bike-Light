import tkinter
import tkintermapview
from compass import CompassWidget
from threading import Thread
import time
import serial
import os


# global utc, status, lat, lon, lat_dir, lon_dir, heading, in_workout
# global lat2, lon2

global lat, lon, speed, heading, start_btn_bit, prev_start_btn_bit, stop_btn_bit, prev_stop_btn_bit, elapsed_time #, distance
lat, lon = 34.02171, -118.28890
speed = 0
heading = 0
time_status_bit = 0
start_btn_bit = 1
prev_start_btn_bit = 1
stop_btn_bit = 1
prev_stop_btn_bit = 1
elapsed_time = 0


ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)


def read_sentence():
    # while ch != '$':
    #     ch = ser.read()
    if ch.decode() == '$':
        while ch.decode() != "\n":
            ch = ser.read()
            sentence += ch.decode()
        print(sentence)
        return sentence.strip("\n")
    return ""


def parse_sentence(sentence):
    global lat, lon, speed, heading, start_btn_bit, prev_start_btn_bit, stop_btn_bit, prev_stop_btn_bit
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
        lat = convert_to_decimal(raw_lat, raw_lat_dir)
        lon = convert_to_decimal(raw_lon, raw_lon_dir)

        heading = raw_heading
        speed = raw_speed * 1.15  # convert from knots to mph

    prev_start_btn_bit = start_btn_bit
    start_btn_bit = raw_btn_1
    
    prev_stop_btn_bit = stop_btn_bit
    stop_btn_bit = raw_btn_2


# converts ddmm.mmmm to decimal degrees
def convert_to_decimal(coord, direction):
    degrees = int(coord[:2])
    minutes = float(coord[2:])
    decimal = degrees + minutes / 60
    if direction in ["S", "W"]:  # adjust for southern and western hemispheres
        decimal = -decimal
    return decimal

example_sentences = [
    "$1,120305,4917.240,N,12310.640,W,5.5,270.0,1,0\n",
    "$1,231547,4832.120,N,0133.540,E,8.7,230.2,0,1\n",
    "$0,045120,3451.980,S,05827.440,W,15.5,120.0,1,1\n",
    "$1,193045,4034.910,N,07358.760,W,20.1,75.4,0,0\n",
    "$1,063210,0142.850,S,03651.120,E,9.9,280.3,0,1\n",
    "$1,080559,5212.670,N,00453.980,E,7.8,180.0,1,0\n",
    "$0,112300,3723.880,N,12658.790,E,4.5,90.0,1,1\n",
    "$1,134500,5515.440,S,06640.520,W,3.6,360.0,0,0\n",
    "$1,152634,2612.560,N,08015.340,E,12.2,310.9,0,1\n",
    "$0,174823,3132.310,N,07428.400,E,13.4,225.5,1,0\n",
    "$1,200907,4716.320,N,00833.650,E,6.7,95.3,0,1\n",
    "$1,221500,6017.150,N,02457.430,E,11.9,125.1,1,1\n",
    "$0,234159,0145.230,N,10350.140,E,16.3,200.6,0,0\n",
    "$1,014510,2212.880,S,04310.770,W,14.7,145.0,0,1\n",
    "$1,035620,5426.440,N,01024.330,W,5.5,235.7,1,0\n",
    "$0,055730,4831.940,S,12345.670,W,8.2,90.5,0,0\n",
    "$1,071840,1934.070,N,09907.310,W,17.8,270.9,0,1\n",
    "$0,090950,3922.560,N,11624.450,E,10.3,355.0,1,1\n",
    "$1,103405,2934.760,N,09522.120,W,19.6,150.4,0,0\n",
    "$1,121517,0250.880,N,10142.550,E,2.4,180.0,1,0\n"
]


def handle_data(root):
    global lat, lon, speed, heading, elapsed_time
    # while True:
    for i in range(len(example_sentences)):
        ser.write(example_sentences[i])
        
        time.sleep(1.0)
        
        if ser.in_waiting > 0:
            sentence = read_sentence()
            print(sentence)
            parse_sentence(sentence)
        else:
            print("No data.")
            continue

        if time_status_bit == 0 and (start_btn_bit != prev_start_btn_bit):
            time_status_bit = 1
            
        if time_status_bit == 1 and (stop_btn_bit != prev_stop_btn_bit):
            time_status_bit = 0
            
        if time_status_bit == 1:
            elapsed_time += 1

        root_tk.event_generate("<<data_ready>>")


disp_width = 600
disp_height = 400

root_tk = tkinter.Tk()

# Frame for the map
frame1 = tkinter.Frame(root_tk, width=2 / 3 * disp_width, height=disp_height)
frame1.grid(row=0, column=0, sticky="nswe")
root_tk.grid_columnconfigure(0, weight=2)  # Allow the map frame to expand more

# Frame for the compass and labels
frame2 = tkinter.Frame(
    root_tk, width=1 / 3 * disp_width, height=disp_height, bg="black", pady=25
)
frame2.grid(row=0, column=1, sticky="nswe")
root_tk.grid_columnconfigure(
    1, weight=1
)  # Ensure frame2 takes up the correct amount of space
frame2.grid_propagate(False)  # Prevents the frame from resizing to fit its contents

# Allows the column within frame2 to expand and center its contents
frame2.grid_columnconfigure(0, weight=1)

# Define colors and fonts
text_color = "#39FF14"  # Neon green hex code
value_font = ("Verdana", 24)  # Larger font size for the value
text_font = ("Verdana", 12)  # Smaller font size for the text

# Speed value and label
speed_value = tkinter.Label(
    frame2, text="0", font=value_font, fg=text_color, bg="black"
)
speed_value.grid(row=0, column=0, sticky="s", padx=20, pady=5)
speed_label = tkinter.Label(
    frame2, text="mph", font=text_font, fg=text_color, bg="black"
)
speed_label.grid(row=1, column=0, sticky="n", padx=20)

# Compass widget
compass_widget = CompassWidget(frame2, width=150)
compass_widget.grid(
    row=2, column=0, sticky="nsew", pady=(20, 20)
)  # Add padding for visual spacing

# Time value and label
time_value = tkinter.Label(
    frame2, text="00:00", font=value_font, fg=text_color, bg="black"
)
time_value.grid(row=3, column=0, sticky="s", padx=20, pady=5)
time_label = tkinter.Label(
    frame2, text="time", font=text_font, fg=text_color, bg="black"
)
time_label.grid(row=4, column=0, sticky="n", padx=20)

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

# def move(lat, lon, i):
#     map_widget.set_position(lat, lon)
#     compass_widget.update_arrow_direction((i + 1) % 360, "E")
#     speed_value.config(text = f"{i % 35}")
#     time_value.config(text=f"{i // 60:02}:{i % 60:02}")
#     # root_tk.after(1000, move, lat, lon + 0.0000125, i + 1)


def move(event):
    global lat, lon, speed, heading, time_status_bit, prev_time_status_bit, elapsed_time
    map_widget.set_position(lat, lon)
    compass_widget.update_arrow_direction(heading, "E")
    speed_value.config(text=f"{speed}")

    hours = elapsed_time // 3600
    remaining_seconds = elapsed_time % 3600
    minutes = remaining_seconds // 60
    seconds = remaining_seconds % 60

    # Format the time into HH:MM:SS
    time_value.config(text=f"{hours:02}:{minutes:02}:{seconds:02}")


root_tk.bind("<<data_ready>>", move)

thread = Thread(target=handle_data, args=(root_tk,))
thread.daemon = True  # Makes sure the thread will close with the application
thread.start()

root_tk.mainloop()
