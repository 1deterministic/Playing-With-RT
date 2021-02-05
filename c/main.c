#define GLFW_WINDOW_TITLE_SIZE 128

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#if USE_OPENCL
    #include <CL/cl.h>
    #include "opencl.h"

#else
    #include <pthread.h>

#endif

#include "util.h"
#include "random.h"
#include "moment.h"
#include "color.h"
#include "vector.h"
#include "ray.h"
#include "material.h"
#include "object.h"
#include "camera.h"
#include "thread.h"
#include "ppm.h"
#include "data.h"
#include "input.h"

int main(int argc, char** argv) {
    data_t self = {0};

    self.depth = 8;
    self.samples = 2;

    self.window_title = malloc(GLFW_WINDOW_TITLE_SIZE * sizeof(char));
    if (self.window_title == NULL) {
        return 1;
    }
    snprintf(self.window_title, GLFW_WINDOW_TITLE_SIZE, "Playing with RT");

    // image as a 1d array
    self.image_width = 512;
    self.image_height = 256;
    self.image = malloc(self.image_width * self.image_height * sizeof(color_t));
    if (self.image == NULL) {
        return 1;
    }

    // camera with fixed lower left corner
    self.camera = camera_fix((camera_t) {
        .vp_width = 2.0f * (float) self.image_width / (float) self.image_height,
        .vp_height = 2.0f,
        .focal_length = 1.0f,
        .position = (vector_t) {
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        },
        .x_axis = (vector_t) {
            .x = 2.0f * (float) self.image_width / (float) self.image_height,
            .y = 0.0f,
            .z = 0.0f
        },
        .y_axis = (vector_t) {
            .x = 0.0f,
            .y = 2.0f,
            .z = 0.0f
        }
    });

    // world objects
    self.world_size = 3;
    self.world = malloc(self.world_size * sizeof(object_t));
    if (self.world == NULL) {
        return 1;
    }

    self.world[0] = (object_t) {
        .type = OBJECT_TYPE_SPHERE,
        .material = (material_t) {
            .color = (color_t) {
                .red = 0.7f,
                .green = 0.7f,
                .blue = 0.7f
            },
            .type = MATERIAL_TYPE_METAL,
            .blur = 0.1f
        },
        .center = (vector_t) {
            .x = -0.5f,
            .y = 0.0f,
            .z = -1.0f
        },
        .radius = 0.5f            
    };
    self.world[1] = (object_t) {
        .type = OBJECT_TYPE_SPHERE,
        .material = (material_t) {
            .color = (color_t) {
                .red = 0.35f,
                .green = 0.35f,
                .blue = 0.35f
            },
            .type = MATERIAL_TYPE_DIFFUSE
        },
        .center = (vector_t) {
            .x = 0.5f,
            .y = 0.0f,
            .z = -1.0f
        },
        .radius = 0.5f            
    };
    self.world[2] = (object_t) {
        .type = OBJECT_TYPE_SPHERE,
        .material = (material_t) {
            .color = (color_t) {
                .red = 0.2f,
                .green = 0.6f,
                .blue = 0.4f
            },
            .type = MATERIAL_TYPE_DIFFUSE
        },
        .center = (vector_t) {
            .x = 0.0f,
            .y = -1000.5f,
            .z = -1.0f
        },
        .radius = 1000.0f
    };

    #if USE_OPENCL
        // opencl related configs        

        // compile cache is a PITA
        setenv("CUDA_CACHE_DISABLE", "1", 1);

        // choose opencl platform and device
        self.opencl.platform = 0;
        self.opencl.device = 0;

        // read the opencl code from kernel.c
        FILE* file = fopen("kernel.c", "r");
        if (file == NULL) {
            return 1;
        }

        // get the file size and allocate source accordingly
        self.opencl.source_size = get_file_size(file);
        self.opencl.source = malloc(self.opencl.source_size * sizeof(char));
        if (self.opencl.source == NULL) {
            return 1;
        }

        // put the entire file in self.opencl.source
        fread(self.opencl.source, sizeof(char), self.opencl.source_size, file);
        fclose(file);

        // get opencl platforms
        self.opencl.error = clGetPlatformIDs(0, NULL, &self.opencl.platforms_size);
        self.opencl.platforms = malloc(self.opencl.platforms_size * sizeof(cl_platform_id));
        if (self.opencl.platforms == NULL) {
            return 1;
        }
        self.opencl.error = clGetPlatformIDs(self.opencl.platforms_size, self.opencl.platforms, &self.opencl.platforms_size);

        // get opencl devices
        self.opencl.devices_size = malloc(self.opencl.platforms_size * sizeof(cl_uint));
        if (self.opencl.devices_size == NULL) {
            return 1;
        }
        self.opencl.devices = malloc(self.opencl.platforms_size * sizeof(cl_device_id*));
        if (self.opencl.devices == NULL) {
            return 1;
        }
        for (int i = 0; i < self.opencl.platforms_size; i++) {
            self.opencl.error = clGetDeviceIDs(self.opencl.platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &self.opencl.devices_size[i]);
            self.opencl.devices[i] = malloc(self.opencl.devices_size[i] * sizeof(cl_device_id));
            if (self.opencl.devices[i] == NULL) {
                return 1;
            }
            self.opencl.error = clGetDeviceIDs(self.opencl.platforms[i], CL_DEVICE_TYPE_ALL, self.opencl.devices_size[i], self.opencl.devices[i], &self.opencl.devices_size[i]);

            // show device info
            for (int j = 0; j < self.opencl.devices_size[i]; j++) {
                printf("%s %d.%d: ", (self.opencl.platform == i && self.opencl.device == j) ? "[*]" : "[ ]", i, j);
                
                char name[512];
                self.opencl.error = clGetDeviceInfo(self.opencl.devices[i][j], CL_DEVICE_NAME, sizeof(name), name, NULL);
                printf("%s ", name);

                cl_uint cu;
                self.opencl.error = clGetDeviceInfo(self.opencl.devices[i][j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cu), &cu, NULL);
                printf("(%d compute units)\n", cu);
            }
        }

        // get device group size
        self.opencl.error = clGetDeviceInfo(self.opencl.devices[self.opencl.platform][self.opencl.device], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(self.opencl.work_size), &self.opencl.work_size, NULL);

        // create context
        self.opencl.context = clCreateContext((cl_context_properties[]) {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties) self.opencl.platforms[self.opencl.platform],
            0
        }, 1, &self.opencl.devices[self.opencl.platform][self.opencl.device], NULL, NULL, &self.opencl.error);

        // create command queue
        self.opencl.command_queue = clCreateCommandQueue(self.opencl.context, self.opencl.devices[self.opencl.platform][self.opencl.device], 0, &self.opencl.error);

        // create and compile program
        self.opencl.program = clCreateProgramWithSource(self.opencl.context, 1, (const char*[]) {
            self.opencl.source
        }, (const size_t[]) {
            self.opencl.source_size
        }, &self.opencl.error);
        self.opencl.error = clBuildProgram(self.opencl.program, 0, NULL, "-I./ -cl-fast-relaxed-math", NULL, NULL);
        if (self.opencl.error) printf("%s\n", opencl_get_error_string(self.opencl.error));

        // create the kernel
        self.opencl.kernel = clCreateKernel(self.opencl.program, "rt", &self.opencl.error);
        // uncomment to show compiler messages
        if (self.opencl.error) {
            char error[65536] = "\0";
            self.opencl.error = clGetProgramBuildInfo(self.opencl.program, self.opencl.devices[self.opencl.platform][self.opencl.device], CL_PROGRAM_BUILD_LOG, sizeof(error), error, NULL);
            printf("%s\n", error);
        }

        // create the memory buffers
        self.opencl.buffers_size = 8;
        self.opencl.buffers = malloc(self.opencl.buffers_size * sizeof(cl_mem));
        if (self.opencl.buffers == NULL) {
            return 1;
        }
        self.opencl.buffers[0] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, sizeof(camera_t), NULL, &self.opencl.error);
        self.opencl.buffers[1] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, self.world_size * sizeof(object_t), NULL, &self.opencl.error);
        self.opencl.buffers[2] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, sizeof(int), NULL, &self.opencl.error);
        self.opencl.buffers[3] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, self.image_width * self.image_height * sizeof(color_t), NULL, &self.opencl.error);
        self.opencl.buffers[4] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, sizeof(int), NULL, &self.opencl.error);
        self.opencl.buffers[5] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, sizeof(int), NULL, &self.opencl.error);
        self.opencl.buffers[6] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, sizeof(int), NULL, &self.opencl.error);
        self.opencl.buffers[7] = clCreateBuffer(self.opencl.context, CL_MEM_READ_WRITE, sizeof(int), NULL, &self.opencl.error);

        // set kernel arguments
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 0, sizeof(self.opencl.buffers[0]), &self.opencl.buffers[0]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 1, sizeof(self.opencl.buffers[1]), &self.opencl.buffers[1]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 2, sizeof(self.opencl.buffers[2]), &self.opencl.buffers[2]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 3, sizeof(self.opencl.buffers[3]), &self.opencl.buffers[3]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 4, sizeof(self.opencl.buffers[4]), &self.opencl.buffers[4]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 5, sizeof(self.opencl.buffers[5]), &self.opencl.buffers[5]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 6, sizeof(self.opencl.buffers[6]), &self.opencl.buffers[6]);
        self.opencl.error = clSetKernelArg(self.opencl.kernel, 7, sizeof(self.opencl.buffers[7]), &self.opencl.buffers[7]);

    #else
        // thread related configs
        self.threads.threads_size = get_nprocs();

        // allocate one thread per cpu
        self.threads.threads = malloc(self.threads.threads_size * sizeof(pthread_t));
        if (self.threads.threads == NULL) {
            return 1;
        }

        // create an arguments object for every thread
        self.threads.arguments = malloc(self.threads.threads_size * sizeof(argument_t));
        if (self.threads.arguments == NULL) {
            return 1;
        }

        for (int i = 0; i < self.threads.threads_size; i++) {
            self.threads.arguments[i] = (argument_t) {
                .id = i,
                .size = self.threads.threads_size,
                .camera = &self.camera,
                .world = self.world,
                .world_size = &self.world_size,
                .image = self.image,
                .image_width = &self.image_width,
                .image_height = &self.image_height,
                .depth = &self.depth,
                .samples = &self.samples
            };
        }

    #endif

    // initialize glfw
    if (!glfwInit()) {
        return 1;
    }

    // set window preferences before creating
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // create the glfw window
    
    GLFWwindow* window = glfwCreateWindow(self.image_width, self.image_height, self.window_title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, &self);
    glfwSetKeyCallback(window, key_callback);

    // the image created will be transformed into a texture
    GLuint texture;
    glGenTextures(1, &texture);
        
    // run until a close command is received
    while(!glfwWindowShouldClose(window)) {
        moment_t frame_start = moment_now();

        if (self.frame_count > 0) {
            snprintf(self.window_title, GLFW_WINDOW_TITLE_SIZE, "Playing with RT - frame: %d in %.2fms (%.2f fps)", self.frame_count, 1000.0f * self.frame_time, 1.0f / self.frame_time);
            glfwSetWindowTitle(window, self.window_title);
        }

        self.world[0].center.z += -0.1f * self.frame_time;
        self.world[1].center.z += -0.05f * self.frame_time;

        #if USE_OPENCL
            // write inputs to the device memory
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[0], CL_FALSE, 0, sizeof(camera_t), &self.camera, 0, NULL, NULL);
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[1], CL_FALSE, 0, self.world_size * sizeof(object_t), self.world, 0, NULL, NULL);
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[2], CL_FALSE, 0, sizeof(int), &self.world_size, 0, NULL, NULL);
            // self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[3], CL_FALSE, 0, self.image_width * self.image_height * sizeof(color_t), self.image, 0, NULL, NULL);
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[4], CL_FALSE, 0, sizeof(int), &self.image_width, 0, NULL, NULL);
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[5], CL_FALSE, 0, sizeof(int), &self.image_height, 0, NULL, NULL);
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[6], CL_FALSE, 0, sizeof(int), &self.depth, 0, NULL, NULL);
            self.opencl.error = clEnqueueWriteBuffer(self.opencl.command_queue, self.opencl.buffers[7], CL_FALSE, 0, sizeof(int), &self.samples, 0, NULL, NULL);

            // run the kernel
            self.opencl.error = clEnqueueNDRangeKernel(self.opencl.command_queue, self.opencl.kernel, 1, NULL, (const size_t[]) {
                self.opencl.work_size < self.image_width * self.image_height ? self.opencl.work_size : self.image_width * self.image_height
            }, NULL, 0, NULL, NULL);

            // read the results from the device memory
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[0], CL_FALSE, 0, sizeof(camera_t), &self.camera, 0, NULL, NULL);
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[1], CL_FALSE, 0, self.world_size * sizeof(object_t), self.world, 0, NULL, NULL);
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[2], CL_FALSE, 0, sizeof(int), &self.world_size, 0, NULL, NULL);
            self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[3], CL_FALSE, 0, self.image_width * self.image_height * sizeof(color_t), self.image, 0, NULL, NULL);
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[4], CL_FALSE, 0, sizeof(int), &self.image_width, 0, NULL, NULL);
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[5], CL_FALSE, 0, sizeof(int), &self.image_height, 0, NULL, NULL);
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[6], CL_FALSE, 0, sizeof(int), &self.depth, 0, NULL, NULL);
            // self.opencl.error = clEnqueueReadBuffer(self.opencl.command_queue, self.opencl.buffers[7], CL_FALSE, 0, sizeof(int), &self.samples, 0, NULL, NULL);

            self.opencl.error = clFinish(self.opencl.command_queue);

        #else
            // start all threads
            for (int i = 0; i < self.threads.threads_size; i++) {
                pthread_create(&self.threads.threads[i], NULL, rt, &self.threads.arguments[i]);
            }

            // stop the main thread until all other threads have finished
            for (int i = 0; i < self.threads.threads_size; i++) {
                pthread_join(self.threads.threads[i], NULL);
            }

        #endif

        // create a 2d texture from the image
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, self.image_width, self.image_height, 0, GL_RGB, GL_FLOAT, self.image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // clear the window
        glViewport(0, 0, self.image_width, self.image_height);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            // create a rectangle with the rt image as a texture and map it to the entire window
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
            glEnd();
        glPopMatrix();
        glFlush();

        glfwSwapBuffers(window);
        glfwPollEvents();

        self.frame_time = moment_difference(frame_start, moment_now());
        self.frame_count++;
    }

    // free everything
    free(self.image);

    #if USE_OPENCL
        free(self.opencl.platforms);
        free(self.opencl.devices);
        free(self.opencl.devices_size);
        free(self.opencl.source);
        free(self.opencl.buffers);

    #else
        free(self.threads.arguments);
        free(self.threads.threads);

    #endif
    
    // terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
