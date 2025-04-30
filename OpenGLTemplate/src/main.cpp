#include "core.h"
using namespace Engine;

void terminateGLFW();
#define PARTICLE_COUNT 1000


const int MAX_POINTS = PARTICLE_COUNT;
GLuint pointsVAO, pointsVBO;
std::vector<Point2D> points(MAX_POINTS);

void simulation_thread();
void SimC_simulation_thread();

bool isRunning = true;

int main() {
    const int windowWidth = 900, windowHeight = 900;
    const bool fullScreenMode = false;
    float pointSize = 4; // Możesz zmieniać ten rozmiar dynamicznie

    std::thread watek(SimC_simulation_thread);
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
    
    float ups = 1000. / sim.step_size;
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

#define GRID_SIZE 32

#define DT 0.01f
#define GRAVITY_MAGNITUDE -9.8f

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    Vec2 pos;
    Vec2 vel;
} Particle;

typedef struct {
    Vec2 velocity;
    float mass;
} GridCell;

Particle particles[PARTICLE_COUNT];
GridCell grid[GRID_SIZE][GRID_SIZE];

void init_particles() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].pos.x = 10.0f + (float)rand() / RAND_MAX * GRID_SIZE;
        particles[i].pos.y = 20.0f + (float)rand() / RAND_MAX * GRID_SIZE;
        particles[i].vel.x = 0.0f;
        particles[i].vel.y = 0.0f;
    }
}

void clear_grid() {
    for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x].velocity = { 0.0f, 0.0f };
            grid[y][x].mass = 0.0f;
        }
}

void particle_to_grid() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle* p = &particles[i];
        int x = (int)p->pos.x;
        int y = (int)p->pos.y;

        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            grid[y][x].velocity.x += p->vel.x;
            grid[y][x].velocity.y += p->vel.y;
            grid[y][x].mass += 1.0f;
        }
    }

    for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x].mass > 0.0f) {
                grid[y][x].velocity.x /= grid[y][x].mass;
                grid[y][x].velocity.y /= grid[y][x].mass;
            }
        }
}

Vec2 gravity = { 0.0f, -9.8f };  

void apply_gravity() {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[y][x].mass > 0.0f) {
                grid[y][x].velocity.x += gravity.x * DT;
                grid[y][x].velocity.y += gravity.y * DT;
            }
        }
    }
}

void set_gravity_direction(Vec2 dir) {
    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (len > 1e-6f) {
        gravity.x = dir.x / len * GRAVITY_MAGNITUDE;
        gravity.y = dir.y / len * GRAVITY_MAGNITUDE;
    }
    else {
        gravity.x = 0.0f;
        gravity.y = 0.0f;
    }
}

void grid_to_particle() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle* p = &particles[i];
        int x = (int)p->pos.x;
        int y = (int)p->pos.y;

        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            Vec2 gridVel = grid[y][x].velocity;
            p->vel = gridVel;
        }
    }
}

void advect_particles() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle* p = &particles[i];

        p->pos.x += p->vel.x * DT;
        p->pos.y += p->vel.y * DT;

        if (p->pos.x < 0) { p->pos.x = 0; p->vel.x *= -0.5f; }
        if (p->pos.x >= GRID_SIZE) { p->pos.x = GRID_SIZE - 0.1; p->vel.x *= -0.5f; }

        if (p->pos.y < 0) { p->pos.y = 0; p->vel.y *= -0.5f; }
        if (p->pos.y >= GRID_SIZE) { p->pos.y = GRID_SIZE - 0.1; p->vel.y *= -0.5f; }
    }
}

void apply_repulsion() {
    const float minDist = 2.0f;
    const float k = 10.0f; 
    const float eps = 0.001f;

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        for (int j = i + 1; j < PARTICLE_COUNT; j++) {
            Vec2 dp = {
                particles[i].pos.x - particles[j].pos.x,
                particles[i].pos.y - particles[j].pos.y
            };
            float dist2 = dp.x * dp.x + dp.y * dp.y;
            if (dist2 < eps) {
                particles[i].vel.x *= -1;
                particles[i].vel.y *= -1;
            }
            if (dist2 < minDist * minDist) {
                float dist = sqrtf(dist2);
                float force = k * (minDist - dist) / dist;

                Vec2 dir = { dp.x / dist, dp.y / dist };
                particles[i].vel.x += dir.x * force * DT;
                particles[i].vel.y += dir.y * force * DT;
                particles[j].vel.x -= dir.x * force * DT;
                particles[j].vel.y -= dir.y * force * DT;
            }

        }
    }
}


void step_simulation() {
    clear_grid();
    particle_to_grid();
    apply_gravity();
    grid_to_particle();
    apply_repulsion();
    advect_particles();
}

void SimC_simulation_thread() {

    init_particles();
    Vec2 gravityDirection = { 0.0f, 1.0f };

    while (isRunning) {
        
        for (int i = 0; i < PARTICLE_COUNT; i++) {
			points[i].x = particles[i].pos.x / 32.;
			points[i].y = particles[i].pos.y / 32.;
		}
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));

        gravityDirection.x = Engine::Input::normX;
        gravityDirection.y = Engine::Input::normY;
        set_gravity_direction(gravityDirection);
        step_simulation();
        
	}
}


