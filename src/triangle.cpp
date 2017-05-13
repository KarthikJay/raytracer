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

	color = Eigen::Vector3d(0.0, 0.0, 0.0); 
	ambient = 0.0;
	diffuse = 0.0;
	specular = 0.0;
	roughness = 0.5;
	metallic = 0.0;
	ior = 1.0;
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
	out << "- Color: {" << this->color.format(SpaceFormat) << "}" << std::endl;
	out << "- Material:" << std::endl;
	out << "  - Ambient: " << this->ambient << std::endl;
	out << "  - Diffuse: " << this->diffuse << std::endl;
	out << "  - Specular: " << this->specular << std::endl;
	out << "  - Roughness: " << this->roughness << std::endl;
	out << "  - Metallic: " << this->metallic << std::endl;
	out << "  - Index of Refraction: " << this->ior << std::endl;
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

// return 0 if no collision
/*
double Triangle::collision(Ray &r)
{
	Eigen::Vector3d edge_0;
	double dist = get_normal().dot(points[0]);
	double time = 0.0;
	// TODO(kjayakum): Put 0.001 as a const double in utility
	bool parallel = (get_normal().dot(r.direction) < 0.001) ? true : false;
	if(!parallel)
	{
		time = -(get_normal().dot(r.origin) + dist) / get_normal().dot(r.direction);
		//std::cout << "Time: " << time << std::endl;
		time = (time < 0.0) ? 0.0 : time;
	}

	// Inside-Outside test
	if(time > 0.0)
	{
		time = (edge_test(points[1], points[0], r, time) &&
				edge_test(points[2], points[1], r, time) &&
				edge_test(points[0], points[2], r, time)) ? time : 0.0;
	}

	return time;
}
*/

double Triangle::collision(Ray &r)
{
	double time, gamma, beta, a;
	Eigen::Matrix3d compute, original;

	original(0, 0) = points[0](0) - points[1](0);
	original(1, 0) = points[0](1) - points[1](1);
	original(2, 0) = points[0](2) - points[1](2);
	original(0, 1) = points[0](0) - points[2](0);
	original(1, 1) = points[0](1) - points[2](1);
	original(2, 1) = points[0](2) - points[2](2);
	original(0, 2) = r.direction(0);
	original(1, 2) = r.direction(1);
	original(2, 2) = r.direction(2);
	a = original.determinant();

	// Setup for t
	compute(0, 0) = points[0](0) - points[1](0);
	compute(1, 0) = points[0](1) - points[1](1);
	compute(2, 0) = points[0](2) - points[1](2);
	compute(0, 1) = points[0](0) - points[2](0);
	compute(1, 1) = points[0](1) - points[2](1);
	compute(2, 1) = points[0](2) - points[2](2);
	compute(0, 2) = points[0](0) - r.origin(0);
	compute(1, 2) = points[0](1) - r.origin(1);
	compute(2, 2) = points[0](2) - r.origin(2);
	time = compute.determinant() / a;

	// Setup for gamma
	compute(0, 0) = points[0](0) - points[1](0);
	compute(1, 0) = points[0](1) - points[1](1);
	compute(2, 0) = points[0](2) - points[1](2);
	compute(0, 1) = points[0](0) - r.origin(0);
	compute(1, 1) = points[0](1) - r.origin(1);
	compute(2, 1) = points[0](2) - r.origin(2);
	compute(0, 2) = r.direction(0);
	compute(1, 2) = r.direction(1);
	compute(2, 2) = r.direction(2);
	gamma = compute.determinant() / a;

	// Setup for beta
	compute(0, 0) = points[0](0) - r.origin(0);
	compute(1, 0) = points[0](1) - r.origin(1);
	compute(2, 0) = points[0](2) - r.origin(2);
	compute(0, 1) = points[0](0) - points[2](0);
	compute(1, 1) = points[0](1) - points[2](1);
	compute(2, 1) = points[0](2) - points[2](2);
	compute(0, 2) = r.direction(0);
	compute(1, 2) = r.direction(1);
	compute(2, 2) = r.direction(2);
	beta = compute.determinant() / a;

	time = (time > 0) ? time : 0.0;
	time = (gamma > 0 && gamma < 1) ? time : 0.0;
	time = (beta > 0 && beta < (1 - gamma)) ? time : 0.0;
	//std::cout << "Time is: " << time << std::endl;

	return time;
}