class Color:
    def __init__(self, red, green, blue):
        self.trim(red, green, blue)

    # keep color values between 0 and 1
    def trim(self, red, green, blue):
        self.red = 1.0 if red > 1.0 else 0.0 if red < 0.0 else red
        self.green = 1.0 if green > 1.0 else 0.0 if green < 0.0 else green
        self.blue = 1.0 if blue > 1.0 else 0.0 if blue < 0.0 else blue

    def __add__(self, other):
        if type(other).__name__ == "Color":
            return Color(
                self.red + other.red,
                self.green + other.green,
                self.blue + other.blue
            )

        elif type(other) == int or type(other) == float:
            return Color(
                self.red + other,
                self.green + other,
                self.blue + other
            )

    def __sub__(self, other):
        if type(other).__name__ == "Color":
            return Color(
                self.red - other.red,
                self.green - other.green,
                self.blue - other.blue
            )

        elif type(other) == int or type(other) == float:
            return Color(
                self.red - other,
                self.green - other,
                self.blue - other
            )

    def __mul__(self, other):
        if type(other).__name__ == "Color":
            return Color(
                self.red * other.red,
                self.green * other.green,
                self.blue * other.blue
            )

        elif type(other) == int or type(other) == float:
            return Color(
                self.red * other,
                self.green * other,
                self.blue * other
            )

    def __truediv__(self, other):
        if type(other).__name__ == "Color":
            return Color(
                self.red / other.red,
                self.green / other.green,
                self.blue / other.blue
            )

        elif type(other) == int or type(other) == float:
            return Color(
                self.red / other,
                self.green / other,
                self.blue / other
            )