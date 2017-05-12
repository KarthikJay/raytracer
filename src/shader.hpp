#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <eigen3/Eigen/Dense>

#include "ray.hpp"
#include "scene.hpp"

enum class BRDF : int
{
	BLINN_PHONG,
	COOK_TORRANCE
};

class Shader
{
public:
	const double epsilon = 0.001;
	// TODO(kjayakum): Put the appropriate variables into private later
	Eigen::Vector3d pixel_color;
	Eigen::Vector3d view_space_point;
	Eigen::Vector3d look_vector;
	// TODO(kjayakum): Re-design the link to the scene better
	Scene *scene;
	BRDF shading;
	Ray pixel_ray;
	uint pixel_x;
	uint pixel_y;

	//Shader();
	Shader(Scene &scene);
	void set_scene(Scene &scene);
	void set_pixel(uint pixel_x, uint pixel_y);
	void set_shading(BRDF type);
	bool is_shadowed(const Ray &shadow, const Light &light) const;
	void print_pixel_ray(std::ostream &out) const;
	void print_pixel_color(std::ostream &out) const;
	void print_collision(std::ostream &out) const;
	void get_pixel_color() const;
	double get_collision_time(const Ray &ray, double time_limit = std::numeric_limits<double>::max()) const;
	std::shared_ptr<Shape> get_collision_shape(const Ray &ray, double time, double delta = 0.000001) const;
	void render(std::string filename) const;
	Eigen::Array3d blinn_phong() const;
protected:
	void update_member_variables();

private:
};

#endif