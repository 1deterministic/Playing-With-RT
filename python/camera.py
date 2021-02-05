from vector import *
from ray import *

class Camera:
    def __init__ (self, vp_width, vp_height, focal_length, position, x_axis, y_axis):
        self.fix(vp_width, vp_height, focal_length, position, x_axis, y_axis)

    # calculates the lower left from the other parameters
    def fix(self, vp_width, vp_height, focal_length, position, x_axis, y_axis):
        self.vp_width = vp_width
        self.vp_height = vp_height
        self.focal_length = focal_length
        self.position = position
        self.x_axis = x_axis
        self.y_axis = y_axis
        self.lower_left = self.position - self.x_axis / 2.0 - self.y_axis / 2.0 - Vector(
            0.0,
            0.0,
            self.focal_length
        )

    # creates a ray from the camera position to some point on an imaginary plane in front of it
    def ray(self, x, y):
        return Ray(
            self.position,
            self.lower_left + (self.x_axis * x) + (self.y_axis * y) - self.position
        )