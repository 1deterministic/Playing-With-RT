import math

class Random:
    def __init__(self, seed = 11):
        self.seed = seed
        
    def rng(self, min, max):
        self.seed = (self.seed * 16807) % 2147483647

        return min + math.fabs((max - min) * self.seed / 2147483647.0)