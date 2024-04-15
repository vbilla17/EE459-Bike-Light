import tkinter as tk
import math

class CompassWidget(tk.Frame):
    def __init__(self, parent, width=200, **kwargs):
        super().__init__(parent, bg='black', highlightthickness=0, **kwargs)  # Ensure the frame has no border
        self.width = width
        self.height = width  # Keep the widget square
        self.canvas = tk.Canvas(self, width=self.width, height=self.height, bg='black', bd=0, highlightthickness=0)
        self.canvas.pack()

        # Dynamically adjust dimensions to fit elements within the widget
        self.circle_diameter = self.width * 0.7  # Leave more padding around the circle
        self.circle_padding = (self.width - self.circle_diameter) / 2
        self.arrow_length = self.circle_diameter * 0.4
        self.tick_length = self.circle_diameter * 0.05
        self.font_size = int(self.width / 15)  # Adjust font size based on widget width
        self.label_offset = self.font_size * 1.5  # Adjust label offset

        self.draw_compass()
        self.update_arrow_direction(0, 'N')

    def draw_compass(self):
        center = self.width / 2
        self.canvas.create_oval(
            self.circle_padding, self.circle_padding,
            self.width - self.circle_padding, self.height - self.circle_padding,
            outline="white")

        # Adjust label positions to ensure visibility within the widget
        label_dist_from_center = self.circle_diameter / 2 + self.label_offset
        directions = {
            'N': (center, center - label_dist_from_center),
            'E': (center + label_dist_from_center, center),
            'S': (center, center + label_dist_from_center),
            'W': (center - label_dist_from_center, center),
        }
        
        for direction, (x, y) in directions.items():
            self.canvas.create_text(x, y, text=direction, font=('Arial', self.font_size, 'bold'), fill='white', anchor=tk.CENTER)

        # Drawing tick marks
        for i in range(24):
            angle = math.radians(i * 15)
            start_x = center + (self.circle_diameter / 2 - self.tick_length) * math.cos(angle)
            start_y = center + (self.circle_diameter / 2 - self.tick_length) * math.sin(angle)
            end_x = center + (self.circle_diameter / 2) * math.cos(angle)
            end_y = center + (self.circle_diameter / 2) * math.sin(angle)
            self.canvas.create_line(start_x, start_y, end_x, end_y, fill='white')
            
        self.canvas.create_oval(center - 3, center - 3, center + 3, center + 3, fill="red", outline="black")


    def update_arrow_direction(self, magnetic_variation, direction):
        angle_radians = math.radians(magnetic_variation)
        if direction.upper() == "W":
            angle_radians = -angle_radians

        center = self.width / 2
        end_x = center + self.arrow_length * math.sin(angle_radians)
        end_y = center - self.arrow_length * math.cos(angle_radians)

        self.canvas.delete('arrow')
        self.canvas.create_line(center, center, end_x, end_y, fill="red", arrow=tk.LAST, width=2, tags='arrow')
