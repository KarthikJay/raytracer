#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <eigen3/Eigen/Dense>

#include "shape.hpp"

class Sphere : public Shape
{
public:
	double radius;
	Eigen::Vector3d center;

	Sphere();
	Sphere(Eigen::Vector3d center, double radius,
			Eigen::Vector3d color, double ambient, double diffuse,
			double specular, double roughness);
	double collision(Ray &r);
	void print_type(std::ostream &out) const;
	Eigen::Vector3d get_normal(Eigen::Vector3d point);
private:
	void print(std::ostream &out) const;
};

#endif