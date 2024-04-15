import tkinter
import tkintermapview
from compass import CompassWidget
from threading import Thread
import time
import serial



global utc, status, lat, lon, lat_dir, lon_dir, heading, in_workout
global lat2, lon2
# in_workout init to false
# when in_workout = False, timer set to 0s, start button press will set it to True, stop button won't do anything
# when in_workout = True, timer set to (time - time@start), start button won't do anything, stop buttonwill set it to False
lat, lon = 34.02171, -118.28890

ser = serial.Serial('/dev/serial0', baudrate=9600, timeout=1)

def handle_data(root):
    global lat2, lon2
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
            lat2 = rv
            lon2 = rv
        else:
            print("No data.")
        time.sleep(1.0)  # Sleep for a bit to avoid spamming the terminal
        
        root_tk.event_generate("<<data_ready>>")

    

disp_width = 600
disp_height = 400

root_tk = tkinter.Tk()

# Frame for the map
frame1 = tkinter.Frame(root_tk, width=2 / 3 * disp_width, height=disp_height)
frame1.grid(row=0, column=0, sticky="nswe")
root_tk.grid_columnconfigure(0, weight=2)  # Allow the map frame to expand more

# Frame for the compass and labels
frame2 = tkinter.Frame(root_tk, width=1 / 3 * disp_width, height=disp_height, bg="black", pady=25)
frame2.grid(row=0, column=1, sticky="nswe")
root_tk.grid_columnconfigure(1, weight=1)  # Ensure frame2 takes up the correct amount of space
frame2.grid_propagate(False)  # Prevents the frame from resizing to fit its contents

# Allows the column within frame2 to expand and center its contents
frame2.grid_columnconfigure(0, weight=1)

# Define colors and fonts
text_color = "#39FF14"  # Neon green hex code
value_font = ('Verdana', 24)  # Larger font size for the value
text_font = ('Verdana', 12)  # Smaller font size for the text

# Speed value and label
speed_value = tkinter.Label(frame2, text="0", font=value_font, fg=text_color, bg="black")
speed_value.grid(row=0, column=0, sticky="s", padx=20, pady=5)
speed_label = tkinter.Label(frame2, text="mph", font=text_font, fg=text_color, bg="black")
speed_label.grid(row=1, column=0, sticky="n", padx=20)

# Compass widget
compass_widget = CompassWidget(frame2, width=150)
compass_widget.grid(row=2, column=0, sticky="nsew", pady=(20, 20))  # Add padding for visual spacing

# Time value and label
time_value = tkinter.Label(frame2, text="00:00", font=value_font, fg=text_color, bg="black")
time_value.grid(row=3, column=0, sticky="s", padx=20, pady=5)
time_label = tkinter.Label(frame2, text="time", font=text_font, fg=text_color, bg="black")
time_label.grid(row=4, column=0, sticky="n", padx=20)

# Map widget
map_widget = tkintermapview.TkinterMapView(frame1, width=500, height=400, corner_radius=0)
map_widget.pack(fill="both", expand=True)
map_widget.set_zoom(18)

# Init poisition for map widget
map_widget.set_position(lat, lon, marker=False)

# Current location dot; always stays in the middle of the map while the map moves
marker_size = 10
marker_canvas = tkinter.Canvas(frame1, width=marker_size, height=marker_size, bg="white", highlightthickness=0)
marker_canvas.create_oval(0, 0, marker_size, marker_size, fill="blue")
marker_canvas.place(relx=0.5, rely=0.5, anchor="center")

# def move(lat, lon, i):
#     map_widget.set_position(lat, lon)
#     compass_widget.update_arrow_direction((i + 1) % 360, "E")
#     speed_value.config(text = f"{i % 35}")
#     time_value.config(text=f"{i // 60:02}:{i % 60:02}")
#     # root_tk.after(1000, move, lat, lon + 0.0000125, i + 1)

def move(event):
    global lat, lon
    map_widget.set_position(lat, lon)    

root_tk.bind("<<data_ready>>", move)

thread = Thread(target=handle_data, args=(root_tk,))
thread.daemon = True # Makes sure the thread will close with the application
thread.start()

root_tk.mainloop()
