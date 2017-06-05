#ifndef BOX_HPP
#define BOX_HPP

#include <eigen3/Eigen/Dense>

#include "shape.hpp"

class Box : public Shape
{
public:
	Eigen::Vector3d min;
	Eigen::Vector3d max;

	Box();
	double collision(Ray &r);
	void print_type(std::ostream &out) const;
	Eigen::Vector3d get_normal(Eigen::Vector3d point);
	Eigen::Vector3d get_world_max_coord();
	Eigen::Vector3d get_world_min_coord();
private:
	void print(std::ostream &out) const;
};

#endif