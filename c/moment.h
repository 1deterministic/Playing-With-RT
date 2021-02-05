typedef struct timespec moment_t;

moment_t moment_now(void) {
    moment_t now;

    clock_gettime(CLOCK_MONOTONIC_RAW, &now);

    return now;
}

float moment_difference(moment_t moment1, moment_t moment2) {
    return (float) ((moment2.tv_sec - moment1.tv_sec) * 1000000 + (moment2.tv_nsec - moment1.tv_nsec) / 1000) / 1000000.0f;
}