import time

class Moment:
    def __init__(self):
        self.time = time.time()

    def now(self):
        self.time = time.time()

    def difference(self, other):
        return self.time - other.time