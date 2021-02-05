class Ray:
    def __init__(self, origin, direction):
        self.origin = origin
        self.direction = direction

    # returns a vector that is the ray scaled by a real parameter
    def at(self, value):
        return self.origin + (self.direction * value)