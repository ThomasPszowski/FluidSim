#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <list>

#include "engine/window.h"
#include "engine/input.h"
#include "engine/shader.h"
#include "engine/buffers.h"
#include "engine/data.h"

#include "simulation/aliases.h"
#include "simulation/debug.h"
#include "simulation/WaterSim.h"

#include <Windows.h>

typedef std::vector<float> vf;
typedef std::vector<std::vector<float>> vvf;

