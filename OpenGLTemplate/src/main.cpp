#include "core.h"
using namespace Engine;

void terminateGLFW();

const int MAX_POINTS = 1000;
GLuint pointsVAO, pointsVBO;
std::vector<Point2D> points(MAX_POINTS);

void simulation_thread();

bool isRunning = true;

int main() {
    const int windowWidth = 900, windowHeight = 900;
    const bool fullScreenMode = false;
    float pointSize = 4; // Możesz zmieniać ten rozmiar dynamicznie

    std::thread watek(simulation_thread);
    //watek.join();

    if (!Window::createWindow(windowWidth, windowHeight, "OpenGL Points", fullScreenMode)) return -1;
    glfwSetInputMode(Window::nativeWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader* shader = nullptr;
    try {
        shader = new Shader("assets/shaders/vertexShader.glsl", "assets/shaders/fragmentShader.glsl");
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        terminateGLFW();
        return -1;
    }
   
    // Tworzenie VAO i VBO dla punktów
    glGenVertexArrays(1, &pointsVAO);
    glGenBuffers(1, &pointsVBO);

    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_POINTS * sizeof(Point2D), points.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Point2D), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Generowanie początkowych punktów
    

    glClearColor(0,0,0,0);

    while (!glfwWindowShouldClose(Window::nativeWindow)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader->use();
        glUniform2f(glGetUniformLocation(shader->getID(), "center"), Input::centerX, Input::centerY);
        glUniform1f(glGetUniformLocation(shader->getID(), "zoom"), Input::zoom);
        glUniform1f(glGetUniformLocation(shader->getID(), "pointSize"), pointSize); // Ustawienie rozmiaru punktu

        glBindVertexArray(pointsVAO);
        glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, MAX_POINTS * sizeof(Point2D), points.data());

        glDrawArrays(GL_POINTS, 0, MAX_POINTS);

        glfwSwapBuffers(Window::nativeWindow);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &pointsVBO);
    glDeleteVertexArrays(1, &pointsVAO);
    delete shader;
	// force the simulation thread to terminate

	isRunning = false;
	watek.join();
    terminateGLFW();
    return 0;
}

void terminateGLFW() {
    glfwDestroyWindow(Window::nativeWindow);
    glfwTerminate();
}


void simulation_thread() {
    using namespace std;
    WaterSim sim(MAX_POINTS);
    sim.step_size = 0.001;
    sim.velocity_damping = 1;
    sim.collision_smoothness = 1;
    sim.collision_strength = 1;
    sim.gravity_vector[0] = 0;
    sim.gravity_vector[1] = 0.1;
    sim.SetGridSize(50);

    sim.ArrangeParticlesSquare();

    int fps = 60;
    int delay = 1000 / fps;

    auto last = chrono::high_resolution_clock::now();
    auto current = chrono::high_resolution_clock::now();

    while (isRunning)
    {
        current = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current - last).count();
        if (duration < delay) {
            this_thread::sleep_for(chrono::milliseconds(delay - duration));
        }
        last = current;
        sim.UpdateSim();
		sim.GenerateOutput(points);

        // check for R key
        if (GetAsyncKeyState(0x52) & 0x8000) {
            RotateVector(sim.gravity_vector, 0.1);
            // display the new gravity vector
            cout << "Gravity vector: (" << sim.gravity_vector[0] << ", " << sim.gravity_vector[1] << ")" << endl;
            // display the angle of the gravity vector
            cout << "Angle: " << atan2(sim.gravity_vector[1], sim.gravity_vector[0]) << endl;
        }


    }

}
