// data structure to encapsulate imporant data that will be passed to the glfw user pointer
typedef struct {
    char* window_title;
    color_t* image;
    int image_width;
    int image_height;
    camera_t camera;
    object_t* world;
    int world_size;
    int depth;
    int samples;    
    int frame_count;
    float frame_time;

    #if USE_OPENCL
        opencl_t opencl;

    #else
        thread_t threads;
        
    #endif
} data_t;