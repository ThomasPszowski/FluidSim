#pragma once

#include "simulation/WaterSim.h"


WaterSim::WaterSim()
{
	this->collision_radius = 1.0 / grid_size;	
	grid = vvlp(grid_size, vlp(grid_size));
	SetVelocityGridSize(grid_size);
	PrepareVelocityGrid();
	particle_vector = vp(particle_count);
}

WaterSim::WaterSim(int particle_count)
{
	this->particle_count = particle_count;
	this->collision_radius = 1.0 / grid_size;
	grid = vvlp(grid_size, vlp(grid_size));
	SetVelocityGridSize(grid_size);
	PrepareVelocityGrid();
	particle_vector = vp(particle_count);
}


void WaterSim::UpdateGrid(Particle& p)
{
	int grid_size = grid.size();
	grid[p.grid_x][p.grid_y].remove(&p);
	p.grid_x = p.x * grid_size;
	p.grid_y = p.y * grid_size;
	grid[p.grid_x][p.grid_y].push_back(&p);
}


void WaterSim::Move(Particle& p) {
	p.x += p.vx * step_size;
	p.y += p.vy * step_size;
	if (p.x >= 1) p.x = 0.9999999;
	else if (p.x < 0) p.x = 0;
	if (p.y >= 1) p.y = 0.9999999;
	else if (p.y < 0) p.y = 0;
}

void WaterSim::MoveWithinCircle(Particle& p) {
	p.x += p.vx * step_size;
	p.y += p.vy * step_size;
	static const float radius = 0.499;
	static const float squared_radius = radius * radius;
	float x_relative_to_center = p.x - radius;
	float y_relative_to_center = p.y - radius;
	float squared_distance = x_relative_to_center * x_relative_to_center + y_relative_to_center * y_relative_to_center;
	if (squared_distance >= squared_radius) {
		float distance = sqrt(squared_distance);
		float factor = radius / distance;
		p.x = radius + x_relative_to_center * factor;
		p.y = radius + y_relative_to_center * factor;
	}
	
}

void WaterSim::Collide(Particle& p1, Particle& p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float squared_distance = dx * dx + dy * dy;
	if (squared_distance < epsilon) {
		p1.vx = (rand() - 1.) / RAND_MAX - 0.5 * same_position_collision_strength;
		p1.vy = (rand() - 1.) / RAND_MAX - 0.5 * same_position_collision_strength;
		return;
	}
	float repulsion = collision_strength / (collision_smoothness * squared_distance + 1);
	p1.vx += repulsion * dx;
	p1.vy += repulsion * dy;
}


void WaterSim::GenerateOutput(bool* pixel_buffer, int buffer_size)
{
	int square_size = buffer_size * buffer_size;
	for (int i = 0; i < square_size; i++)
	{
		pixel_buffer[i] = false;
	}
	for (Particle& p : particle_vector) {
		int pixel_x = p.x * buffer_size;
		int pixel_y = p.y * buffer_size;

		pixel_buffer[pixel_x + pixel_y * buffer_size] = true;
	}
}

void WaterSim::GenerateOutput(std::vector<Point2D>& pixel_buffer)
{
	auto it = particle_vector.begin();
	for (Point2D& p : pixel_buffer) {
		p.x = it->x;
		p.y = it->y;
		it++;
	}
	
}


void WaterSim::ArrangeParticlesRandom()
{
	for (Particle& p : particle_vector) {
		p.x = (rand() - 1.) / RAND_MAX;
		p.y = (rand() - 1.) / RAND_MAX;
		p.vx = (rand() - 1.) / RAND_MAX - 0.5;
		p.vy = (rand() - 1.) / RAND_MAX - 0.5;
		UpdateGrid(p);
	}
}


void WaterSim::ArrangeParticlesSquare()
{
	int square_root = sqrt(particle_count);
	float spacing = 0.3 / square_root;
	for (int i = 0; i < square_root; i++) {
		for (int j = 0; j < square_root; j++) {
			Particle& p = particle_vector[i * square_root + j];
			p.x = 0.5 + (i - square_root / 2) * spacing;
			p.y = 0.5 + (j - square_root / 2) * spacing;
			p.vx = 0;
			p.vy = 0;
			UpdateGrid(p);
		}
	}
	
}

void WaterSim::LimitVelocity(float max_squared_v) {

	for (Particle& p : particle_vector) {
		float squared_v = p.vx * p.vx + p.vy * p.vy;
		if (squared_v > max_squared_v) {
			float factor = sqrt(max_squared_v / squared_v);
			p.vx *= factor;
			p.vy *= factor;
		}
	}
}


void WaterSim::UpdateSim()
{
	for (Particle& p : particle_vector) {

		p.vx += gravity_vector[0];
		p.vy += gravity_vector[1];
		p.vx *= velocity_damping;
		p.vy *= velocity_damping;

	}

	for (Particle& p : particle_vector) {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				int grid_x = p.grid_x + i;
				int grid_y = p.grid_y + j;
				if (grid_x >= 0 && grid_x < grid_size && grid_y >= 0 && grid_y < grid_size) {
					for (Particle* p2 : grid[grid_x][grid_y]) {
						if (p2 != &p) {
							Collide(p, *p2);
						}
					}
				}
			}
		}
	}
	

	for (Particle& p : particle_vector) {
		Move(p);
		//MoveWithinCircle(p);
		UpdateGrid(p);
	}
}


void WaterSim::SetGridSize(int size)
{
	grid_size = size;
	grid = vvlp(grid_size, vlp(grid_size));
	this->collision_radius = 1.0 / grid_size;
	for (Particle& p : particle_vector) {
		UpdateGrid(p);
	}
}

void WaterSim::SetVelocityGridSize(int size)
{
	velocity_grid_size = size;
	combined_velocities = vvvc(velocity_grid_size, vvc(velocity_grid_size));
	horizontal_velocities = vvf(velocity_grid_size + 1, vf(velocity_grid_size + 1));
	vertical_velocities = vvf(velocity_grid_size + 1, vf(velocity_grid_size + 1));
	velocity_cell_size = 1.0 / velocity_grid_size;
}

void WaterSim::PrepareVelocityGrid()
{
	for (int i = 0; i < velocity_grid_size; i++) {
		for (int j = 0; j < velocity_grid_size; j++) {
			combined_velocities[i][j].up = &vertical_velocities[i+1][j];
			combined_velocities[i][j].down = &vertical_velocities[i][j];
			combined_velocities[i][j].left = &horizontal_velocities[i][j];
			combined_velocities[i][j].right = &horizontal_velocities[i][j + 1];
		}
	}
	for (int i = 0; i < velocity_grid_size; i++) {
		combined_velocities[velocity_grid_size - 1][i].up = nullptr;
		combined_velocities[0][i].down = nullptr;
		combined_velocities[i][0].left = nullptr;
		combined_velocities[i][velocity_grid_size - 1].right = nullptr;
	}
}

void WaterSim::TransferVelocitiesToGrid()
{
	for (vvc& row : combined_velocities) {
		for (VelocityCell& cell : row) {
			if (cell.up) {
				*cell.up = 0;
			}
			if (cell.down) {
				*cell.down = 0;
			}
			if (cell.left) {
				*cell.left = 0;
			}
			if (cell.right) {
				*cell.right = 0;
			}
			cell.number_of_particles = 0;
		}
	}
	for (Particle& p : particle_vector) {
		int j = p.x * velocity_grid_size;
		int i = p.y * velocity_grid_size;
		float dx = p.x * velocity_grid_size - j;
		float dy = p.y * velocity_grid_size - i;
		combined_velocities[i][j].number_of_particles++;
		
		if (combined_velocities[i][j].up and combined_velocities[i][j].down) {
			*combined_velocities[i][j].down += p.vy * dy;
			*combined_velocities[i][j].up += p.vy * (1.0 - dy);
		}
		else if (combined_velocities[i][j].up) {
			*combined_velocities[i][j].up += p.vy;
		}
		else if (combined_velocities[i][j].down) {
			*combined_velocities[i][j].down += p.vy;
		}
		
		if (combined_velocities[i][j].left and combined_velocities[i][j].right) {
			*combined_velocities[i][j].left += p.vx * dx;
			*combined_velocities[i][j].right += p.vx * (1.0 - dx);
		}
		else if (combined_velocities[i][j].left) {
			*combined_velocities[i][j].left += p.vx;
		}
		else if (combined_velocities[i][j].right) {
			*combined_velocities[i][j].right += p.vx;
		}
	}
}

void WaterSim::ReCalculateVelocities() {

}

void WaterSim::TransferVelocitiesToParticles() {
	for (Particle& p : particle_vector) {
		int j = p.x * velocity_grid_size;
		int i = p.y * velocity_grid_size;
		float dx = p.x * velocity_grid_size - j;
		float dy = p.y * velocity_grid_size - i;

		if (combined_velocities[i][j].up and combined_velocities[i][j].down) {
			p.vy = *combined_velocities[i][j].down * dy + *combined_velocities[i][j].up * (1.0 - dy);
		}
		else if (combined_velocities[i][j].up) {
			p.vy = *combined_velocities[i][j].up;
		}
		else if (combined_velocities[i][j].down) {
			p.vy = *combined_velocities[i][j].down;
		}
		
	}
}