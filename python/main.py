import timeit
import math

from OpenGL.GL import *
import glfw

from urandom import *
from moment import *
from color import *
from vector import *
from ray import *
from material import *
from object import *
from camera import *
from thread import *
from ppm import *
from data import *
from input import *

if __name__ == "__main__":
    self = Data()

    self.window_title = "Playing with RT"
    self.image_width = 720
    self.image_height = 480
    self.image = list(range(self.image_width * self.image_height * 3))
    self.camera = Camera(
        2.0 * self.image_width / self.image_height,
        2.0,
        1.0,
        Vector(
            0.0,
            0.0,
            0.0
        ),
        Vector(
            2.0 * self.image_width / self.image_height,
            0.0,
            0.0
        ),
        Vector(
            0.0,
            2.0,
            0.0
        )
    )
    self.world_size = 3
    self.world = world = [
        Sphere(
            Metal(
                Color(
                    0.7,
                    0.7,
                    0.7
                ),
                0.1
            ),
            Vector(
                -0.5,
                0.0,
                -1.0
            ),
            0.5
        ),
        Sphere(
            Diffuse(
                Color(
                    0.35,
                    0.35,
                    0.35
                )
            ),
            Vector(
                0.5,
                0.0,
                -1.0
            ),
            0.5
        ),
        Sphere(
            Diffuse(
                Color(
                    0.2,
                    0.6,
                    0.4
                ),
            ),
            Vector(
                0.0,
                -1000.5,
                -1.0
            ),
            1000.0
        )
    ]
    self.depth = 4
    self.samples = 1
    self.frame_count = 0
    self.frame_time = 0
    self.threads_size = 8
    self.threads = []

    for i in range(0, self.threads_size):
        self.threads.append(Thread(
            i,
            self.threads_size,
            self.camera,
            self.world,
            self.world_size,
            self.image,
            self.image_width,
            self.image_height,
            self.depth,
            self.samples
        ))

    glfw.init()

    window = glfw.create_window(self.image_width, self.image_height, self.window_title, None, None)

    glfw.make_context_current(window)
    glfw.set_window_user_pointer(window, self)
    glfw.set_key_callback(window, key_callback)

    texture = list(range(0, 1))
    glGenTextures(1, texture)

    while not glfw.window_should_close(window):
        frame_start = Moment()

        # start all threads
        for i in self.threads:
            i.start()

        # stop the main thread until all other threads have finished
        for i in self.threads:
            i.join()

        glViewport(0, 0, self.image_width, self.image_height)
        glClear(GL_COLOR_BUFFER_BIT)

        glBindTexture(GL_TEXTURE_2D, texture[0])
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, self.image_width, self.image_height, 0, GL_RGB, GL_FLOAT, self.image)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)

        glEnable(GL_TEXTURE_2D)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()

        glBegin(GL_QUADS)
        glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, 1.0)
        glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 1.0)
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 1.0)
        glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 1.0)
        glEnd()
        glFlush()

        glfw.swap_buffers(window)
        glfw.poll_events()

        frame_time = Moment().difference(frame_start)
        print(frame_time)

    glfw.terminate()