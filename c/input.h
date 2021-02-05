void key_callback(GLFWwindow* window, int key, int code, int action, int mods) {
    data_t* self = (data_t*) glfwGetWindowUserPointer(window);

    // take a screenshot on F2 press
    if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
        ppm_create("./screenshot.ppm", self->image, self->image_width, self->image_height);
    }
}