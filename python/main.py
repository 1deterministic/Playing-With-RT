import glfw

if __name__ == "__main__":
    glfw.init()

    window = glfw.create_window(800, 600, "Playing with RT", None, None)
    glfw.make_context_current(window)

    while not glfw.window_should_close(window):
        glfw.swap_buffers(window)
        glfw.poll_events()

    glfw.terminate()