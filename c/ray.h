typedef struct {
    vector_t origin;
    vector_t direction;
} ray_t;

// returns a vector that is the ray scaled by a real parameter
vector_t ray_at(ray_t ray, float value) {
    return vector_add_vv(ray.origin, vector_mul_vf(ray.direction, value));
}