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
			Eigen::Vector3d color, double ambient, double diffuse);
	double collision(Ray &r);
	void print_type();
private:
	void print(std::ostream &out) const;
};

#endif