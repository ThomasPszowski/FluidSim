#pragma once
#include <iostream>
#include <list>
#include <vector>

struct Point2D {
	float x, y;
};

class WaterSim
{
private:
	int particle_count = 400;
	int grid_size = 50;
	float collision_radius;
public:
	struct Particle
	{
	public:
		float x = 0, y = 0, vx = 0, vy = 0;
		int grid_x = 0, grid_y = 0;
		Particle() {}
		Particle(float x, float y, float vx, float vy) : x(x), y(y), vx(vx), vy(vy) {}
	};
	typedef std::vector<std::vector<std::list<WaterSim::Particle*>>> vvlp;
	typedef std::vector<std::list<WaterSim::Particle*>> vlp;
	typedef std::list<WaterSim::Particle*> lp;
	typedef std::vector<WaterSim::Particle> vp;
	
	float epsilon = 0.000001;
	float collision_strength = 1;
	float collision_smoothness = 50;
	float step_size = 0.001;
	float velocity_damping = 0.99;
	float gravity_vector[2] = { 0, 0.1 };
	vvlp grid;
	vp particle_vector;

	WaterSim();

	WaterSim(int particle_count);

	void UpdateGrid(Particle& p);

	void Move(Particle& p);

	void UpdateSim();

	void Collide(Particle& p1, Particle& p2);

	void GenerateOutput(bool* pixel_buffer, int output_size);

	void GenerateOutput(std::vector<Point2D>& pixel_buffer);

	void ArrangeParticlesRandom();

	void ArrangeParticlesSquare();

	void SetGridSize(int size);
};



