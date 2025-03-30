#pragma once
#include "core.h"

namespace Engine {
	namespace Input {
		extern float zoom;
		extern int maxIter;
		extern bool keyPressedData[GLFW_KEY_LAST];
		extern bool mouseButtonPressedData[GLFW_MOUSE_BUTTON_LAST];
		extern float moveSpeed;
		extern float centerX;
		extern float centerY;
		extern float mouseX;
		extern float mouseY;
		extern float mouseScrollX;
		extern float mouseScrollY;
		extern float scrollButtonX;
		extern float scrollButtonY;
		extern float normX;
		extern float normY;

		// Handle user input
		void handleKeyInput();

		// Utility
		bool isKeyDown(int key);
		bool isMouseButtonDown(int mouseButton);

		// Callback
		void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
		void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	}
}