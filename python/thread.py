import threading

from urandom import *
from color import *
from vector import *
from ray import *
from material import *
from object import *
from camera import *

class Thread:
    def __init__(self, id, size, camera, world, world_size, image, image_width, image_height, depth, samples):        
        self.id = id
        self.size = size
        self.camera = camera
        self.world = world
        self.world_size = world_size
        self.image = image
        self.image_width = image_width
        self.image_height = image_height
        self.depth = depth
        self.samples = samples        

    def start(self):
        self.thread = threading.Thread(target = self.rt, args = ())
        self.thread.start()

    def join(self):
        self.thread.join()

    def rt(self):
        seed = Random(self.image_height - self.id * self.image_width)

        # start from pixel = id and then jump size pixels on each iteration
        for pixel in range(self.id, self.image_width * self.image_height, self.size):
            # since image is a 1d array, convert the index to x and y positions
            x = pixel % self.image_width
            y = pixel // self.image_height

            # starts with a black pixel
            color = Color(
                0.0,
                0.0,
                0.0
            )

            # get samples amount of colors from sleightly different positions and average them to reduce image noise
            for sample in range(0, self.samples):
                # generate a random offset for x and y
                i = (x + seed.rng(0.0, 1.0)) / (self.image_width - 1)
                j = (self.image_height + seed.rng(0.0, 1.0) - y - 1) / self.image_height

                # create a ray based on these offsets
                ray = self.camera.ray(i, j)

                # starts with a white pixel
                partial = Color(
                    1.0,
                    1.0,
                    1.0
                )

                # use the camera position as the last hit point when calculating the nearest hit
                last_hit_point = self.camera.position

                # get the ray color
                for bounce in range(0, self.depth):
                    # starts with a ray miss
                    hit = Hit(True, None, None, 0, None)

                    # walk the objects array in search for a ray hit
                    for object in self.world:
                        candidate = object.hit(ray, 0.001, 1000)
                        hit = hit.nearest(last_hit_point, candidate)

                    if hit.missed:
                        # makes a blue gradient simulating the sky
                        vertical = (ray.direction.y + 1.0) * 0.5
                        start = Color(
                            1.0,
                            0.64,
                            0.0,
                        )
                        end = Color(
                            0.46,
                            0.35,
                            0.74
                        )

                        # tint the current color with the calculated sky color
                        partial = partial * (start * (1.0 - vertical) + end * vertical)

                        # stop looking for more hits
                        break
                    
                    else:
                        # update the last hit point
                        last_hit_point = hit.point

                        # tint the current color with the one from the ray hit
                        partial = partial * hit.material.color

                        # scatter the ray to use on the next iteration
                        ray = hit.material.scatter(ray, hit.point, hit.normal, seed)

                # add the color divided by the number of samples to the pixel color
                color = color + partial / self.samples

            # add the final pixel color to the image
            self.image[pixel * 3 + 0] = color.red
            self.image[pixel * 3 + 1] = color.green
            self.image[pixel * 3 + 2] = color.blue