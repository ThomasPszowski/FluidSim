#include "engine/input.h"
#include "engine/window.h"
#include <iomanip>

namespace Engine {
	namespace Input {
		
		float zoom = 1;  
		// 50190208 
		int maxIter = 100;
		bool keyPressedData[GLFW_KEY_LAST] = {};
		bool mouseButtonPressedData[GLFW_MOUSE_BUTTON_LAST] = {};
		float moveSpeed = 0.05f;
		float centerX = -0.743643887037158704752191506114774;
		float centerY = 0.131825904205311970493132056385139;
		float mouseX = 0.0f;
		float mouseY = 0.0f;
		float mouseScrollX = 0.0f;
		float mouseScrollY = 0.0f;

		// Utility
		bool isKeyDown(int key) {
			if (key >= 0 && key < GLFW_KEY_LAST) {
				return keyPressedData[key];
			}
			return false;
		}

		bool isMouseButtonDown(int mouseButton) {
			if (mouseButton >= 0 && mouseButton < GLFW_MOUSE_BUTTON_LAST) {
				return mouseButtonPressedData[mouseButton];
			}
			return false;
		}

		// Callbacks
		void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (key >= 0 && key < GLFW_KEY_LAST) {
				if (action == GLFW_PRESS || action == GLFW_REPEAT) {
					keyPressedData[key] = true;
				}
				else if (action == GLFW_RELEASE) {
					keyPressedData[key] = false;
				}
			}
		}

		void mousePosCallback(GLFWwindow* window, double xpos, double ypos) {
			mouseX = (float)xpos;
			mouseY = (float)ypos;
		}

		void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				// Pobierz rozmiary okna
				int width, height;
				glfwGetWindowSize(window, &width, &height);

				// Przelicz współrzędne ekranu na [-1,1]
				float normX = (mouseX / width) * 2.0f - 1.0f;
				float normY = 1.0f - (mouseY / height) * 2.0f;  // Odwracamy oś Y

				// Przekształć do współrzędnych Mandelbrota
				Input::centerX += normX * 1.5 / Input::zoom;
				Input::centerY += normY * 1.5 / Input::zoom;

				// wyswietl dokladne wspolrzedne
				std::cout << std::fixed << std::setprecision(8);
				std::cout << "centerX: " << Input::centerX << " centerY: " << Input::centerY << std::endl;
				std::cout << "zoom: " << Input::zoom << std::endl;
			}
		}


		void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
			mouseScrollX = (float)xoffset;
			mouseScrollY = (float)yoffset;

			// Zwiększ/zmniejsz zoom w zależności od scrolla
			if (yoffset > 0) {
				Engine::Input::zoom *= 1.1f;  // Zwiększenie zoomu
			}
			else if (yoffset < 0) {
				Engine::Input::zoom *= 0.9f;  // Zmniejszenie zoomu
			}
		}

		void handleKeyInput() {
			float delta = moveSpeed / zoom;  // Prędkość zależna od zoomu

			if (isKeyDown(GLFW_KEY_LEFT)) {
				centerX -= delta;
			}
			if (isKeyDown(GLFW_KEY_RIGHT)) {
				centerX += delta;
			}
			if (isKeyDown(GLFW_KEY_UP)) {
				centerY += delta;
			}
			if (isKeyDown(GLFW_KEY_DOWN)) {
				centerY -= delta;
			}
			if (isKeyDown(GLFW_KEY_EQUAL)) {
				maxIter += 2;
			}
			if (isKeyDown(GLFW_KEY_MINUS)) {
				maxIter -= 2;
			}
			if (isKeyDown(GLFW_KEY_ESCAPE)) {
				Window::close();
			}
		}
	}
}