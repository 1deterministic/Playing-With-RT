#define PPM_BUFFER_SIZE 16

int ppm_create(char* path, color_t* image, int image_width, int image_height) {
    FILE* file = fopen(path, "w");
    if (!file) {
        return 1;
    }

    char* buffer = malloc(PPM_BUFFER_SIZE * sizeof(char));
    if (!buffer) {
        return 1;
    }

    sprintf(buffer, "P3\n%d %d\n255\n", image_width, image_height);
    fwrite(buffer, 1, strlen(buffer), file);

    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            sprintf(buffer, "%d %d %d ",
                (int) (image[i + j * image_width].red * 255.0f),
                (int) (image[i + j * image_width].green * 255.0f),
                (int) (image[i + j * image_width].blue * 255.0f)
            );
            fwrite(buffer, 1, strlen(buffer), file);
        }

        fwrite("\n", 1, 1, file);
    }

    fclose(file);
    free(buffer);

    return 0;
}