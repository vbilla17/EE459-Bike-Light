import tkinter
import tkintermapview

disp_width = 600
disp_height = 400

# create tkinter window
root_tk = tkinter.Tk()
# root_tk.geometry(f"{1000}x{700}")
root_tk.title("map_view_simple_example.py")
frame1 = tkinter.ttk.Frame(
    root_tk, width=2 / 3 * disp_width, height=disp_height, borderwidth=5, relief="ridge"
)
frame2 = tkinter.ttk.Frame(
    root_tk, width=1 / 3 * disp_width, height=disp_height, borderwidth=5, relief="ridge"
)
frame2.grid_propagate(False)
speed_lbl = tkinter.ttk.Label(frame2, text="speed", borderwidth=5)
speed_lbl_2 = tkinter.ttk.Label(frame2, text="speed2", borderwidth=5)
speed_lbl_3 = tkinter.ttk.Label(frame2, text="speed3", borderwidth=5)

# create map widget
map_widget = tkintermapview.TkinterMapView(frame1, width=500, height=400, corner_radius=0)
map_widget.pack(fill="both", expand=True)

# set other tile server (standard is OpenStreetMap)
# map_widget.set_tile_server("https://mt0.google.com/vt/lyrs=m&hl=en&x={x}&y={y}&z={z}&s=Ga", max_zoom=22)  # google normal
# map_widget.set_tile_server("https://mt0.google.com/vt/lyrs=s&hl=en&x={x}&y={y}&z={z}&s=Ga", max_zoom=22)  # google satellite

# set current position and zoom
# map_widget.set_position(52.516268, 13.377695, marker=False)  #
map_widget.set_zoom(16)

# set current position with address
# map_widget.set_address("Berlin Germany", marker=False)

def marker_click(marker):
    print(f"marker clicked - text: {marker.text}  position: {marker.position}")

# set a position marker (also with a custom color and command on click)
marker_2 = map_widget.set_marker(52.516268, 13.377695, text="Brandenburger Tor", command=marker_click)
marker_3 = map_widget.set_marker(52.55, 13.4, text="52.55, 13.4")
# marker_3.set_position(...)
# marker_3.set_text(...)
# marker_3.delete()

# set a path
# path_1 = map_widget.set_path([marker_2.position, marker_3.position, (52.568, 13.4), (52.569, 13.35)])
# path_1.add_position(...)
# path_1.remove_position(...)
# path_1.delete()
c1 = 38.9398
c2 = -77.0737
map_widget.set_position(c1, c2, marker=False)
def move(c1, i):
    map_widget.delete_all_marker()
    map_widget.set_marker(c1, c2)
    # if (i % 10 == 0):
    map_widget.set_position(c1 + 0.0001, c2)
    speed_val = i # read_byte() or something
    c1 = c1 + 0.001
    speed_lbl.config(text = f"val1: {speed_val}")
    speed_lbl_2.config(text = f"val2: {(i*4) % 5}")
    speed_lbl_3.config(text = f"val3: {(i*3) % 5}")
    root_tk.after(2000, move, c1 + 0.00001, i + 1)
    
frame1.grid(row=0, column=0, columnspan=1)
frame2.grid(row=0, column=1, rowspan=3)
speed_lbl.grid(row=0, column=0)
speed_lbl_2.grid(row=1, column=0)
speed_lbl_3.grid(row=2, column=0)


move(38.9398, 0)

root_tk.mainloop()