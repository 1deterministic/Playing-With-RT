#include "random.h"
#include "color.h"
#include "vector.h"
#include "ray.h"
#include "material.h"
#include "object.h"
#include "camera.h"

kernel void rt(global camera_t* camera, global object_t* world, global int* world_size, global color_t* image, global int* image_width, global int* image_height, global int* depth, global int* samples) {
    int id = get_global_id(0);
    int size = get_global_size(0);
    int seed = *image_height - id * *image_width;
    
    // the for loop below is literally copy/pasted from the thread function

    // start from pixel = id and then jump size pixels on each iteration
    for (int pixel = id; pixel < *image_width * *image_height; pixel += size) {
        // since image is a 1d array, convert the index to x and y positions
        int x = pixel % *image_width;
        int y = pixel / *image_width;

        // starts with a black pixel
        color_t color = (color_t) {
            .red = 0.0f,
            .green = 0.0f,
            .blue = 0.0f
        };

        // get samples amount of colors from sleightly different positions and average them to reduce image noise
        for (int sample = 0; sample < *samples; sample++) {
            // generate a random offset for x and y
            float i = ((float) x + rng(0.0f, 1.0f, &seed)) / (float) (*image_width - 1);
            float j = ((float) *image_height + rng(0.0f, 1.0f, &seed) - (float) (y - 1)) / (float) *image_height;

            // create a ray based on these offsets
            ray_t ray = camera_ray(*camera, i, j);

            // starts with a white pixel
            color_t partial = (color_t) {
                .red = 1.0f,
                .green = 1.0f,
                .blue = 1.0f
            };

            // use the camera position as the last hit point when calculating the nearest hit
            vector_t last_hit_point = camera->position;

            // get the ray color
            for (int bounce = 0; bounce < *depth; bounce++) {
                // starts with a ray miss
                hit_t hit = (hit_t) {
                    .missed = 1
                };

                // walk the objects array in search for a ray hit
                for (int i = 0; i < *world_size; i++) {
                    hit_t candidate = object_hit(world[i], ray, 0.001f, 1000.0f);
                    hit = hit_nearest(last_hit_point, hit, candidate);
                }

                if (hit.missed) {
                    // makes a blue gradient simulating the sky
                    float vertical = (ray.direction.y + 1.0f) * 0.5f;
                    color_t start = (color_t) {
                        .red = 1.0f,
                        .green = 0.64f,
                        .blue = 0.0f
                    };
                    color_t end = (color_t) {
                        .red = 0.46f,
                        .green = 0.35f,
                        .blue = 0.74f
                    };

                    // tint the current color with the calculated sky color
                    partial = color_mul_cc(partial, color_add_cc(color_mul_cf(start, 1.0f - vertical), color_mul_cf(end, vertical)));

                    // stop looking for more hits
                    break;
                } else {
                    // update the last hit point
                    last_hit_point = hit.point;

                    // tint the current color with the one from the ray hit
                    partial = color_mul_cc(partial, hit.material.color);

                    // scatter the ray to use on the next iteration
                    ray = material_scatter(hit.material, ray, hit.point, hit.normal, &seed);
                }
            }            

            // add the color divided by the number of samples to the pixel color
            color = color_add_cc(color, color_div_cf(partial, (float) *samples));
        }

        // add the final pixel color to the image
        image[pixel] = color;
    }
}