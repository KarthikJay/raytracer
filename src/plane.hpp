#ifndef PLANE_HPP
#define	PLANE_HPP

#include <eigen3/Eigen/Dense>

#include "shape.hpp"

class Plane : public Shape
{
public:
	double distance;
	Eigen::Vector3d normal;

	Plane();
	Plane(Eigen::Vector3d normal, double distance,
			Eigen::Vector3d color, double ambient, double diffuse);
	double collision(Ray &r);
	void print_type();
private:
	void print(std::ostream &out) const;
};

#endif