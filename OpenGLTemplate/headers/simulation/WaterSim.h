#pragma once
#include "core.h"

struct Point2D {
	float x, y;
};

struct VelocityCell {
	std::vector<float>* up, * down, * left, * right;
	int number_of_particles = 0;
};

class WaterSim
{
private:
	int particle_count = 400;
	int grid_size = 50;
	int velocity_grid_size = 16;
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
	typedef std::vector<float> vf;
	typedef std::vector<std::vector<float>> vvf;
	typedef std::vector<std::vector<std::vector<float>>> vvvf;
	typedef std::vector<VelocityCell> vvc;
	typedef std::vector<std::vector<VelocityCell>> vvvc;
	
	float epsilon = 0.00001;
	float collision_strength = 1;
	float same_position_collision_strength = 1;
	float collision_smoothness =1;
	float step_size = 0.001;
	float velocity_damping = 0.99;
	float gravity_vector[2] = { 0, 0.1 };

	vvlp grid;
	vp particle_vector;
	vvvf horizontal_velocities;
	vvvf vertical_velocities;
	vvvc combined_velocities;
	float velocity_cell_size = 1.0 / velocity_grid_size;


	WaterSim();

	WaterSim(int particle_count);

	void UpdateGrid(Particle& p);

	void PrepareVelocityGrid();

	void Move(Particle& p);

	void MoveWithinCircle(Particle& p);

	void UpdateSim();

	void Collide(Particle& p1, Particle& p2);

	void GenerateOutput(bool* pixel_buffer, int output_size);

	void GenerateOutput(std::vector<Point2D>& pixel_buffer);

	void ArrangeParticlesRandom();

	void ArrangeParticlesSquare();

	void SetGridSize(int size);

	void SetVelocityGridSize(int size);

	void LimitVelocity(float max_v);

	void TransferVelocitiesToGrid();

	void ReCalculateVelocities();

	void TransferVelocitiesToParticles();

};



