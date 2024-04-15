import tkinter
import tkintermapview
from compass import CompassWidget
from threading import Thread
import time
import serial
import os


# global utc, status, lat, lon, lat_dir, lon_dir, heading, in_workout
# global lat2, lon2

global lat, lon, speed, heading, time_status_bit, prev_time_status_bit, time_counter
lat, lon = 34.02171, -118.28890
speed = 0
heading = 0
time_status_bit = 0
prev_time_status_bit = 0
time_counter = 0

# in_workout init to false
# when in_workout = False, timer set to 0s, start button press will set it to True, stop button won't do anything
# when in_workout = True, timer set to (time - time@start), start button won't do anything, stop buttonwill set it to False


ser = serial.Serial("/dev/serial0", baudrate=9600, timeout=1)


def read_sentence():
    ch = ser.read()
    if ch.decode() == "L":
        while ch.decode() != "\n":
            ch = ser.read()
            sentence += ch.decode()
        print(sentence)
        return "L" + sentence.strip("\n")


# def parse_sentence(sentence):
#     global lat, lon, speed, heading, time_status_bit, prev_time_status_bit, time_counter
#     # sentence format:
#     # $<lat>,<lat_dir>,<lon>,<lon_dir>,<speed>,<heading>,<start_stop>\n
#     # $ddmm.mmmm,d,ddmm.mmmm,d,kk.kkk,dd.dd,s\n
#     # Split the sentence by commas into components
#     (
#         raw_lat,
#         raw_lat_dir,
#         raw_lon,
#         raw_lon_dir,
#         raw_speed,
#         raw_heading,
#         raw_start_stop,
#     ) = sentence.split(",")

#     lat = convert_to_decimal(raw_lat, raw_lat_dir)
#     lon = convert_to_decimal(raw_lon, raw_lon_dir)

#     heading = raw_heading
#     speed = raw_speed * 1.15  # convert from knots to mph
#     prev_time_status_bit = time_status_bit
#     time_status_bit = raw_start_stop

def parse_sentence(sentence):
    global lat, lon, speed, heading#, time_status_bit, prev_time_status_bit, time_counter
    # Split the sentence by the labels into components
    parts = sentence.split(", ")

    # Extract and parse each component based on its label
    for part in parts:
        if 'Lat:' in part:
            value, direction = part.replace("Lat: ", "").split()
            lat = convert_to_decimal(value, direction)
        elif 'Lon:' in part:
            value, direction = part.replace("Lon: ", "").split()
            lon = convert_to_decimal(value, direction)
        elif 'Speed:' in part:
            knot_speed = part.replace("Speed: ", "")
            speed = knot_speed * 1.15 # Assuming speed is in knots
        elif 'Heading:' in part:
            raw_heading = part.replace("Heading: ", "")
            heading = raw_heading

    # return gps_data



# converts ddmm.mmmm to decimal degrees
def convert_to_decimal(coord, direction):
    degrees = int(coord[:2])
    minutes = float(coord[2:])
    decimal = degrees + minutes / 60
    if direction in ["S", "W"]:  # adjust for southern and western hemispheres
        decimal = -decimal
    return decimal

example_sentences = [
    "Lat: 3751.65 N, Lon: 12225.42 W, Speed: 12.3, Heading: 145.6\n",
    "Lat: 4832.12 N, Lon: 0133.54 E, Speed: 8.7, Heading: 230.2\n",
    "Lat: 3451.98 S, Lon: 05827.44 W, Speed: 15.5, Heading: 120.0\n",
    "Lat: 4034.91 N, Lon: 07358.76 W, Speed: 20.1, Heading: 75.4\n",
    "Lat: 0142.85 S, Lon: 03651.12 E, Speed: 9.9, Heading: 280.3\n",
    "Lat: 5212.67 N, Lon: 00453.98 E, Speed: 7.8, Heading: 180.0\n",
    "Lat: 3723.88 N, Lon: 12658.79 E, Speed: 4.5, Heading: 90.0\n",
    "Lat: 5515.44 S, Lon: 06640.52 W, Speed: 3.6, Heading: 360.0\n",
    "Lat: 2612.56 N, Lon: 08015.34 E, Speed: 12.2, Heading: 310.9\n",
    "Lat: 3132.31 N, Lon: 07428.40 E, Speed: 13.4, Heading: 225.5\n",
    "Lat: 4716.32 N, Lon: 00833.65 E, Speed: 6.7, Heading: 95.3\n",
    "Lat: 6017.15 N, Lon: 02457.43 E, Speed: 11.9, Heading: 125.1\n",
    "Lat: 0145.23 N, Lon: 10350.14 E, Speed: 16.3, Heading: 200.6\n",
    "Lat: 2212.88 S, Lon: 04310.77 W, Speed: 14.7, Heading: 145.0\n",
    "Lat: 5426.44 N, Lon: 01024.33 W, Speed: 5.5, Heading: 235.7\n",
    "Lat: 4831.94 S, Lon: 12345.67 W, Speed: 8.2, Heading: 90.5\n",
    "Lat: 1934.07 N, Lon: 09907.31 W, Speed: 17.8, Heading: 270.9\n",
    "Lat: 3922.56 N, Lon: 11624.45 E, Speed: 10.3, Heading: 355.0\n",
    "Lat: 2934.76 N, Lon: 09522.12 W, Speed: 19.6, Heading: 150.4\n",
    "Lat: 0250.88 N, Lon: 10142.55 E, Speed: 2.4, Heading: 180.0\n"
]

def handle_data(root):
    global lat, lon, speed, heading, elapsed_time
    i = 0
    while i < len(example_sentences):
        if ser.in_waiting > 0:
            sentence = read_sentence()
            print(sentence)
            parse_sentence(sentence)
        else:
            print("No data.")

        if time_status_bit == 1:
            time_counter += 1
        if time_status_bit == 0 and prev_time_status_bit == 1:
            time_counter = 0

        root_tk.event_generate("<<data_ready>>")
        
        time.sleep(1.0)  # getting data every second
        
        ser.write(example_sentences[i])


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
    global lat, lon, speed, heading, time_status_bit, prev_time_status_bit, time_counter
    map_widget.set_position(lat, lon)
    compass_widget.update_arrow_direction(heading, "E")
    speed_value.config(text=f"{speed}")

    hours = 0 # time_counter // 3600
    remaining_seconds = 0 # time_counter % 3600
    minutes = 0 # remaining_seconds // 60
    seconds = 0 # remaining_seconds % 60

    # Format the time into HH:MM:SS
    time_value.config(text=f"{hours:02}:{minutes:02}:{seconds:02}")


root_tk.bind("<<data_ready>>", move)

thread = Thread(target=handle_data, args=(root_tk,))
thread.daemon = True  # Makes sure the thread will close with the application
thread.start()

root_tk.mainloop()
