#include "simulation/debug.h"

void Draw(bool* buffer, int buffer_size) {
	system("cls");
	for (int i = 0; i < buffer_size; i++) {
		for (int j = 0; j < buffer_size; j++) {
			if (buffer[i * buffer_size + j]) {
				std::cout << "XX";
			}
			else {
				std::cout << ". ";
			}
		}
		std::cout << std::endl;
	}
}

void RotateVector(float* vector, float angle) {
	float x = vector[0];
	float y = vector[1];
	vector[0] = x * cos(angle) - y * sin(angle);
	vector[1] = x * sin(angle) + y * cos(angle);
}
