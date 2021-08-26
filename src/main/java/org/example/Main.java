package org.example;

import org.lwjgl.glfw.GLFWErrorCallback;
import org.lwjgl.opengl.GL;

import static org.lwjgl.glfw.Callbacks.glfwFreeCallbacks;
import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;

public class Main implements Runnable, AutoCloseable {
    public long hwnd;

    public static void main(String[] args) {
        try (Main main = new Main()) {
            main.start();
        }
    }

    public void resize(int w, int h) {
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, w, h, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
    }

    public void start() {
        GLFWErrorCallback.createPrint().set();
        glfwInit();
        hwnd = glfwCreateWindow(800, 600, "lwjgl-basic", 0, 0);
        glfwSetKeyCallback(hwnd, (window, key, scancode, action, mods) -> {
            if (action == GLFW_PRESS) {
                if (key == GLFW_KEY_ESCAPE) {
                    glfwSetWindowShouldClose(window, true);
                }
            }
        });
        glfwSetFramebufferSizeCallback(hwnd, (window, width, height) -> {
            resize(width, height);
        });
        glfwMakeContextCurrent(hwnd);
        GL.createCapabilities();
        glClearColor(0, 0, 0, 1);
        resize(800, 600);
        glfwSwapInterval(1);
        run();
    }

    @Override
    public void run() {
        while (!glfwWindowShouldClose(hwnd)) {
            glClear(GL_COLOR_BUFFER_BIT);
            render();
            glfwSwapBuffers(hwnd);
            glfwPollEvents();
        }
    }

    public void render() {}

    @Override
    public void close() {
        glfwFreeCallbacks(hwnd);
        glfwDestroyWindow(hwnd);
        glfwTerminate();
        glfwSetErrorCallback(null).free();
    }
}
