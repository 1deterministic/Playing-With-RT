typedef struct {
    float red;
    float green;
    float blue;
} color_t;

// keep color values between 0 and 1
color_t color_trim(color_t color) {
    return (color_t) {
        .red = color.red > 1.0f ? 1.0f : color.red < 0.0f ? 0.0f : color.red,
        .green = color.green > 1.0f ? 1.0f : color.green < 0.0f ? 0.0f : color.green,
        .blue = color.blue > 1.0f ? 1.0f : color.blue < 0.0f ? 0.0f : color.blue
    };
}

// operations with another color
color_t color_add_cc(color_t color1, color_t color2) {
    return color_trim((color_t) {
        .red = color1.red + color2.red,
        .green = color1.green + color2.green,
        .blue = color1.blue + color2.blue
    });
}

color_t color_sub_cc(color_t color1, color_t color2) {
    return color_trim((color_t) {
        .red = color1.red - color2.red,
        .green = color1.green - color2.green,
        .blue = color1.blue - color2.blue
    });
}

color_t color_mul_cc(color_t color1, color_t color2) {
    return color_trim((color_t) {
        .red = color1.red * color2.red,
        .green = color1.green * color2.green,
        .blue = color1.blue * color2.blue
    });
}

color_t color_div_cc(color_t color1, color_t color2) {
    return color_trim((color_t) {
        .red = color1.red / color2.red,
        .green = color1.green / color2.green,
        .blue = color1.blue / color2.blue
    });
}

// operations with a number
color_t color_add_cf(color_t color, float value) {
    return color_trim((color_t) {
        .red = color.red + value,
        .green = color.green + value,
        .blue = color.blue + value
    });
}

color_t color_sub_cf(color_t color, float value) {
    return color_trim((color_t) {
        .red = color.red - value,
        .green = color.green - value,
        .blue = color.blue - value
    });
}

color_t color_mul_cf(color_t color, float value) {
    return color_trim((color_t) {
        .red = color.red * value,
        .green = color.green * value,
        .blue = color.blue * value
    });
}

color_t color_div_cf(color_t color, float value) {
    return color_trim((color_t) {
        .red = color.red / value,
        .green = color.green / value,
        .blue = color.blue / value
    });
}