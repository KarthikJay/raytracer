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
			Eigen::Vector3d color, double ambient, double diffuse,
			double specular, double roughness);
	double collision(Ray &r);
	Eigen::Vector3d get_normal(Eigen::Vector3d point);
	void print_type(std::ostream &out) const;
	Eigen::Vector3d get_world_max_coord();
	Eigen::Vector3d get_world_min_coord();
	Eigen::Vector3d get_center();
private:
	void print(std::ostream &out) const;
};

#endif