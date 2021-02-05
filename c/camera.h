typedef struct {
    float vp_width;
    float vp_height;    
    float focal_length;
    vector_t position;
    vector_t x_axis;
    vector_t y_axis;
    vector_t lower_left;
} camera_t;

// calculates the lower left from the other parameters
camera_t camera_fix(camera_t camera) {
    return (camera_t) {
        .vp_width = camera.vp_width,
        .vp_height = camera.vp_height,
        .focal_length = camera.focal_length,
        .position = camera.position,
        .x_axis = camera.x_axis,
        .y_axis = camera.y_axis,
        .lower_left = vector_sub_vv(vector_sub_vv(vector_sub_vv(camera.position, vector_div_vf(camera.x_axis, 2.0f)), vector_div_vf(camera.y_axis, 2.0f)), (vector_t) {
            .x = 0.0f,
            .y = 0.0f,
            .z = camera.focal_length
        })
    };
}

// creates a ray from the camera position to some point on an imaginary plane in front of it
ray_t camera_ray(camera_t camera, float x, float y) {
    return (ray_t) {
        .origin = camera.position,
        .direction = vector_add_vv(camera.lower_left, vector_add_vv(vector_mul_vf(camera.x_axis, x), vector_sub_vv(vector_mul_vf(camera.y_axis, y), camera.position)))
    };
}