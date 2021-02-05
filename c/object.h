#define OBJECT_TYPE_SPHERE 0

typedef  struct {
    int missed;
    vector_t point;
    vector_t normal;
    float where;
    material_t material;
} hit_t;

typedef struct {
    int type;
    material_t material;
    vector_t center;
    float radius;
} object_t;

hit_t hit_nearest(vector_t point, hit_t hit1, hit_t hit2) {
    if (hit1.missed && hit2.missed) {
        return hit1;
    } else if (!hit1.missed && hit2.missed) {
        return hit1;
    } else if (hit1.missed && !hit2.missed) {
        return hit2;
    } else if (vector_length(vector_sub_vv(hit1.point, point)) < vector_length(vector_sub_vv(hit2.point, point))) {
        return hit1;
    } else {
        return hit2;
    }
}

hit_t object_hit(object_t object, ray_t ray, float min, float max) {
    switch(object.type) {
        case OBJECT_TYPE_SPHERE: {
            vector_t oc = vector_sub_vv(ray.origin, object.center);

            float a = vector_dot(ray.direction, ray.direction);
            float b = 2.0f * vector_dot(oc, ray.direction);
            float c = vector_dot(oc, oc) - pow(object.radius, 2);
            float d = pow(b, 2) - 4.0f * a * c;            

            // if no solution is possible, returns a miss
            if (d < 0.0f) {
                return (hit_t) {
                    .missed = 1
                };
            }

            float root = (-1.0f * b - sqrt(d)) / (2.0f * a);

            // check both eq roots and if the ray hits outside the boundaries, returns a miss
            if (root < min || root > max) {                
                root = (-1.0f * b + sqrt(d)) / (2.0f * a);

                if (root < min || root > max) {                            
                    return (hit_t) {
                        .missed = 1
                    };
                }
            }
            
            vector_t point = ray_at(ray, root);
            vector_t normal = vector_unit(vector_sub_vv(point, object.center));

            // invert the vector if it points inside the object
            if (vector_dot(ray.direction, ray.direction) < 0.0f) {
                normal = vector_mul_vf(normal, -1.0f);
            }            

            return (hit_t) {
                .missed = 0,
                .point = point,
                .normal = normal,
                .where = root,
                .material = object.material
            };
        } default: {
            return (hit_t) {
                .missed = 1
            };
        };
    }
}