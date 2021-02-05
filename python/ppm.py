class PPM:
    def __init__(self):
        pass

    def create(self, path, image, image_width, image_height):
        file = open(path, "w")

        file.write(f"P3\n{image_width} {image_height}\n255\n")

        for j in range(0, image_height):
            for i in range(0, image_width):
                file.write(f"{(int) (255 * image[(i + j * image_width) * 3 + 0])} {(int) (255 * image[(i + j * image_width) * 3 + 1])} {(int) (255 * image[(i + j * image_width) * 3 + 2])} ")
            
            file.write("\n")

        file.close()