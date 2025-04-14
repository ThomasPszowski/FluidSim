#include "core.h"
using namespace Engine;

void terminateGLFW();

const int MAX_POINTS = 200;
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
   
    Vertex centerPoint[] = {
    { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) } // White point at center
    };

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

    // Create a VBO, VAO for the center point (new VBO for the point)
    GLuint vaoID_center = Buffers::createVAO();
    GLuint vboID_center = 0; // You will create this buffer
    GLuint bindingIndex_center = 1;  // A new binding index for the point buffer
    GLsizeiptr centerVertexByteSize = sizeof(centerPoint);

    // Create the VBO
    Buffers::createVBO(vaoID_center, centerVertexByteSize, centerPoint, bindingIndex_center, sizeof(Vertex) / sizeof(float), GL_STATIC_DRAW);

    // Add vertex attributes for the point (Position and Color)
    Buffers::addVertexAttrib(vaoID_center, 0, 3, offsetof(Vertex, position), bindingIndex_center);  // Position
    Buffers::addVertexAttrib(vaoID_center, 1, 4, offsetof(Vertex, color), bindingIndex_center);
    

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
    sim.velocity_damping = 0.99;
    sim.collision_smoothness = 100;
    sim.collision_strength = 0.5;
	sim.same_position_collision_strength = 1;
    sim.gravity_vector[0] = 0;
    sim.gravity_vector[1] = 0.1;
    sim.SetGridSize(40);
	sim.SetVelocityGridSize(40);
	float gravity_multiplier = 0.01;

    sim.ArrangeParticlesSquare();
    
    float ups = 100. / sim.step_size;
	int u_delay = 1000 / ups;
	auto u_last = chrono::high_resolution_clock::now();

    int fps = 100;
    int f_delay = 1000 / fps;
    auto f_last = chrono::high_resolution_clock::now();

    auto current = chrono::high_resolution_clock::now();

    while (isRunning)
    {
        current = chrono::high_resolution_clock::now();
        auto f_duration = chrono::duration_cast<chrono::milliseconds>(current - f_last).count();
		auto u_duration = chrono::duration_cast<chrono::milliseconds>(current - u_last).count();
		if (u_duration > u_delay) {
			u_last = current;
			sim.UpdateSim();
		}
        if (f_duration > f_delay) {
            f_last = current;
            sim.GenerateOutput(points);
        }
        sim.gravity_vector[0] = Engine::Input::normX * gravity_multiplier;
		sim.gravity_vector[1] = Engine::Input::normY * gravity_multiplier;
    }

}
