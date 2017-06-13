#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <eigen3/Eigen/Dense>
#include <array>

#include "shape.hpp"

class Triangle : public Shape
{
public:
	std::array<Eigen::Vector3d, 3> points;

	Triangle();
	double collision(Ray &r);
	void print_type(std::ostream &out) const;
	Eigen::Vector3d get_normal(Eigen::Vector3d point = Eigen::Vector3d(0, 0, 0));
	Eigen::Vector3d get_world_max_coord();
	Eigen::Vector3d get_world_min_coord();
	Eigen::Vector3d get_center();
private:
	void print(std::ostream &out) const;
	bool edge_test(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, Ray &r, double time);
};

#endif