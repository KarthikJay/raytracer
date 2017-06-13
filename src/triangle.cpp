#include <math.h>
#include <algorithm>
#include <iostream>

#include "triangle.hpp"

Triangle::Triangle()
{
	for(Eigen::Vector3d itr : points)
	{
		itr = Eigen::Vector3d(0.0, 0.0, 0.0);
	}
	set_material_defaults();
}

void Triangle::print_type(std::ostream &out) const
{
	out << "Triangle";
}

void Triangle::print(std::ostream &out) const
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Type: Triangle" << std::endl;
	for(uint i = 0; i < points.size(); i++)
	{
		out << "- Point[" << i << "]: {" << this->points[i].format(SpaceFormat) << "}" << std::endl;
	}
	this->print_material(out);
}

Eigen::Vector3d Triangle::get_normal(Eigen::Vector3d point)
{
	Eigen::Vector3d ab = points[1] - points[0];
	Eigen::Vector3d ac = points[2] - points[0];

	return ab.cross(ac).normalized();
}

bool Triangle::edge_test(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, Ray &r, double time)
{
	Eigen::Vector3d collision_point = r.get_point(time);
	Eigen::Vector3d edge = p2 - p1;
	Eigen::Vector3d collision_edge = collision_point - p2;
	Eigen::Vector3d perpendicular_dir = edge.cross(collision_edge);
	bool within = get_normal().dot(perpendicular_dir) >= 0 ? true : false;

	return within;
}

Eigen::Vector3d Triangle::get_world_max_coord()
{
	Eigen::Vector3d world_max = Eigen::Vector3d::Zero();
	for(Eigen::Vector3d itr : points)
	{
		world_max(0) = std::max(world_max(0), itr(0));
		world_max(1) = std::max(world_max(1), itr(1));
		world_max(2) = std::max(world_max(2), itr(2));
	}
	world_max << (transform * (Eigen::Vector4d() << world_max.head<3>(), 1).finished()).head<3>();
	return world_max;
}

Eigen::Vector3d Triangle::get_world_min_coord()
{
	Eigen::Vector3d world_min = Eigen::Vector3d::Zero();
	for(Eigen::Vector3d itr : points)
	{
		world_min(0) = std::min(world_min(0), itr(0));
		world_min(1) = std::min(world_min(1), itr(1));
		world_min(2) = std::min(world_min(2), itr(2));
	}
	world_min << (transform * (Eigen::Vector4d() << world_min.head<3>(), 1).finished()).head<3>();
	return world_min;
}

double Triangle::collision(Ray &r)
{
	double time, gamma, beta, a;
	Eigen::Matrix3d compute, original;

	original << points[0](0) - points[1](0), points[0](1) - points[1](1), points[0](2) - points[1](2),
				points[0](0) - points[2](0), points[0](1) - points[2](1), points[0](2) - points[2](2),
				r.direction(0), r.direction(1), r.direction(2);
	a = original.determinant();

	// Setup for t
	compute = original;
	compute.row(2) << points[0](0) - r.origin(0), points[0](1) - r.origin(1), points[0](2) - r.origin(2);
	time = compute.determinant() / a;

	// Setup for gamma
	compute = original;
	compute.row(1) << points[0](0) - r.origin(0), points[0](1) - r.origin(1), points[0](2) - r.origin(2);
	gamma = compute.determinant() / a;

	// Setup for beta
	compute = original;
	compute.row(0) << points[0](0) - r.origin(0), points[0](1) - r.origin(1), points[0](2) - r.origin(2);
	beta = compute.determinant() / a;

	time = (time > 0) ? time : 0.0;
	time = (gamma > 0 && gamma < 1) ? time : 0.0;
	time = (beta > 0 && beta < (1 - gamma)) ? time : 0.0;

	return time;
}