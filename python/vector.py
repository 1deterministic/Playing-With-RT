import math

class Vector:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def __add__(self, other):
        if type(other).__name__ == "Vector":
            return Vector(
                self.x + other.x, 
                self.y + other.y, 
                self.z + other.z
            )

        elif type(other) == int or type(other) == float:
            return Vector(
                self.x + other,
                self.y + other,
                self.z + other
            )

    def __sub__(self, other):
        if type(other).__name__ == "Vector":
            return Vector(
                self.x - other.x,
                self.y - other.y,
                self.z - other.z
            )

        elif type(other) == int or type(other) == float:
            return Vector(
                self.x - other,
                self.y - other,
                self.z - other
            )

    def __mul__(self, other):
        if type(other).__name__ == "Vector":
            return Vector(
                self.x * other.x,
                self.y * other.y,
                self.z * other.z
            )

        elif type(other) == int or type(other) == float:
            return Vector(
                self.x * other,
                self.y * other,
                self.z * other
            )

    def __truediv__(self, other):
        if type(other).__name__ == "Vector":
            return Vector(
                self.x / other.x,
                self.y / other.y,
                self.z / other.z
            )

        elif type(other) == int or type(other) == float:
            return Vector(
                self.x / other,
                self.y / other,
                self.z / other
            )

    def length(self):
        return math.sqrt(self.x ** 2 + self.y ** 2 + self.z ** 2)

    def unit(self):
        return self / self.length()
    
    # % is the dot product
    def __mod__(self, other):
        return self.x * other.x + self.y * other.y + self.z * other.z

    # & is the cross product
    def __and__(self, other):
        return Vector(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x
        )