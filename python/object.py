import math

class Hit:
    def __init__(self, missed, point, normal, where, material):
        self.missed = missed
        self.point = point
        self.normal = normal
        self.where = where
        self.material = material

    def nearest(self, point, other):
        if self.missed and other.missed:
            return self
        elif not self.missed and other.missed:
            return self
        elif self.missed and not other.missed:
            return other
        elif (self.point - point).length() < (other.point - point).length():
            return self
        else:
            return other

class Object:
    def __init__(self):
        pass
    
    def hit(self, ray, min, max):
        return Hit(True, None, None, 0, None)

class Sphere(Object):
    def __init__(self, material, center, radius):
        self.material = material
        self.center = center
        self.radius = radius

    def hit(self, ray, min, max):
        oc = ray.origin - self.center
        
        a = ray.direction % ray.direction
        b = 2.0 * (oc % ray.direction)
        c = (oc % oc) - self.radius ** 2
        d = b ** 2 - 4.0 * a * c
        
        # if no solution is possible, returns a miss
        if d < 0.0:
            return Hit(True, None, None, 0, None)

        root = (-1.0 * b - math.sqrt(d)) / (2.0 * a)

        # check both eq roots and if the ray hits outside the boundaries, returns a miss
        if root < min or root > max:
            root = (-1.0 * b + math.sqrt(d)) / (2.0 * a)

            if root < min or root > max:
                return Hit(True, None, None, 0, None)
        
        point = ray.at(root)
        normal = (point - self.center).unit()

        # invert the vector if it points inside the object
        if ray.direction % ray.direction < 0.0:
            normal = normal * -1.0

        return Hit(False, point, normal, root, self.material)