#define MATERIAL_TYPE_DIFFUSE 0
#define MATERIAL_TYPE_METAL 1

typedef struct {
    color_t color;
    int type;
    float blur;
} material_t;

// returns a ray going from the hit point to some direction based on the properties of the material
ray_t material_scatter(material_t material, ray_t ray, vector_t point, vector_t normal, int* seed) {
    switch(material.type) {
        case MATERIAL_TYPE_DIFFUSE: {
            // get a random unit size vector
            vector_t unit = vector_unit((vector_t) {
                .x = rng(-1.0f, 1.0f, seed),
                .y = rng(-1.0f, 1.0f, seed),
                .z = rng(-1.0f, 1.0f, seed)
            });

            // invert the vector if it points inside the object
            if (vector_dot(unit, normal) < 0.0f) {
                unit = vector_mul_vf(unit, -1.0f);
            }

            vector_t direction = vector_add_vv(normal, unit);

            // prevent the ray from being absorved
            if (vector_length(direction) < 0.001f) {
                direction = normal;
            }

            return (ray_t) {
                .origin = point,
                .direction = direction
            };
        } case MATERIAL_TYPE_METAL: {
            // get a random unit size vector
            vector_t unit = vector_unit((vector_t) {
                .x = rng(-1.0f, 1.0f, seed),
                .y = rng(-1.0f, 1.0f, seed),
                .z = rng(-1.0f, 1.0f, seed)
            });

            // invert the vector if it points inside the object
            if (vector_dot(unit, normal) < 0.0f) {
                unit = vector_mul_vf(unit, -1.0f);
            }

            vector_t direction = vector_add_vv(normal, unit);

            // prevent the ray from being absorved
            if (vector_length(direction) < 0.001f) {
                direction = normal;
            }

            direction = vector_add_vv(vector_mul_vf(unit, material.blur), vector_sub_vv(vector_sub_vv(ray.direction, ray.origin), vector_mul_vf(normal, 2.0f * vector_dot(vector_sub_vv(ray.direction, ray.origin), normal))));

            return (ray_t) {
                .origin = point,
                .direction = direction
            };
        } default: {
            return (ray_t) {
                .origin = point,
                .direction = normal
            };
        };
    }
}