import glfw

from ppm import *

def key_callback(window, key, code, action, mods):
    self = glfw.get_window_user_pointer(window)
    
    # take a screenshot on F2 press
    if key == glfw.KEY_F2 and action == glfw.PRESS:
        PPM().create("./screenshot.ppm", self.image, self.image_width, self.image_height)