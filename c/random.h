// can't use rand() in opencl kernel, so this will be an approximation (https://stackoverflow.com/a/15108013)
float rng(float min, float max, int* seed) {
    *seed = (*seed * 16807) % 2147483647;

    return min + fabs((max - min) * (float) *seed / (float) 2147483647);
}