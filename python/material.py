from urandom import *
from vector import *
from ray import *

class Material:
    def __init__(self, color):
        self.color = color

    # returns a ray going from the hit point to some direction based on the properties of the material
    def scatter(self, ray, point, normal, seed):
        return Ray(point, normal)

class Diffuse(Material):
    def __init__(self, color):
        super().__init__(color)

        self.color = color
    
    # returns a ray going from the hit point to some direction based on the properties of the material
    def scatter(self, ray, point, normal, seed):
        # get a random unit size vector
        unit = Vector(
            seed.rng(-1.0, 1.0),
            seed.rng(-1.0, 1.0),
            seed.rng(-1.0, 1.0)
        ).unit()

        # invert the vector if it points inside the object
        if unit % normal < 0.0:
            unit = unit * -1.0

        direction = normal + unit
        
        # prevent the ray from being absorved
        if direction.length() < 0.001:
            direction = normal

        return Ray(point, direction)

class Metal(Material):
    def __init__(self, color, blur):
        super().__init__(color)

        self.color = color
        self.blur = blur
    
    # returns a ray going from the hit point to some direction based on the properties of the material
    def scatter(self, ray, point, normal, seed):
        # get a random unit size vector
        unit = Vector(
            seed.rng(-1.0, 1.0),
            seed.rng(-1.0, 1.0),
            seed.rng(-1.0, 1.0)
        ).unit()

        # invert the vector if it points inside the object
        if unit % normal < 0.0:
            unit = unit * -1.0

        direction = ((unit * self.blur) + ((ray.direction - ray.origin) - (normal * 2.0 * ((ray.direction - ray.origin) % normal))))
        
        # prevent the ray from being absorved
        if direction.length() < 0.001:
            direction = normal

        return Ray(point, direction)