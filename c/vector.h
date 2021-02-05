typedef struct {
    float x;
    float y;
    float z;
} vector_t;

// operations with another vector
vector_t vector_add_vv(vector_t vector1, vector_t vector2) {
    return (vector_t) {
        .x = vector1.x + vector2.x,
        .y = vector1.y + vector2.y,
        .z = vector1.z + vector2.z
    };
}
vector_t vector_sub_vv(vector_t vector1, vector_t vector2) {
    return (vector_t) {
        .x = vector1.x - vector2.x,
        .y = vector1.y - vector2.y,
        .z = vector1.z - vector2.z
    };
}
vector_t vector_mul_vv(vector_t vector1, vector_t vector2) {
    return (vector_t) {
        .x = vector1.x * vector2.x,
        .y = vector1.y * vector2.y,
        .z = vector1.z * vector2.z
    };
}
vector_t vector_div_vv(vector_t vector1, vector_t vector2) {
    return (vector_t) {
        .x = vector1.x / vector2.x,
        .y = vector1.y / vector2.y,
        .z = vector1.z / vector2.z
    };
}

// operations with a number
vector_t vector_add_vf(vector_t vector, float value) {
    return (vector_t) {
        .x = vector.x + value,
        .y = vector.y + value,
        .z = vector.z + value
    };
}
vector_t vector_sub_vf(vector_t vector, float value) {
    return (vector_t) {
        .x = vector.x - value,
        .y = vector.y - value,
        .z = vector.z - value
    };
}
vector_t vector_mul_vf(vector_t vector, float value) {
    return (vector_t) {
        .x = vector.x * value,
        .y = vector.y * value,
        .z = vector.z * value
    };
}
vector_t vector_div_vf(vector_t vector, float value) {
    return (vector_t) {
        .x = vector.x / value,
        .y = vector.y / value,
        .z = vector.z / value
    };
}

// other vector operations
float vector_length(vector_t vector) {
    return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

vector_t vector_unit(vector_t vector) {
    return vector_div_vf(vector, vector_length(vector));
}

float vector_dot(vector_t vector1, vector_t vector2) {
    return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
}

vector_t vector_cross(vector_t vector1, vector_t vector2) {
    return (vector_t) {
        .x = vector1.y * vector2.z - vector1.z * vector2.y,
        .y = vector1.z * vector2.x - vector1.x * vector2.z,
        .z = vector1.x * vector2.y - vector1.y * vector2.x,
    };
}