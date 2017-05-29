#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <eigen3/Eigen/Dense>
#include <vector>
#include <iostream>

#include "ray.hpp"

class Shape
{
public:
	// TODO(kjayakum): Figure out translations/skews and rotations.
	double ambient;
	double diffuse;
	double specular;
	double roughness;
	double reflection;
	double refraction;
	double metallic;
	double ior;
	double filter;
	Eigen::Vector3d color;
	Eigen::Matrix4d inverse_transform;

	virtual void print_type(std::ostream &out) const = 0;
	void print_material(std::ostream &out) const;
	void set_material_defaults();
	virtual double collision(Ray &r) = 0;
	virtual Eigen::Vector3d get_normal(Eigen::Vector3d point) = 0;
	friend std::ostream &operator<< (std::ostream &out, const Shape &s)
	{
		s.print(out);
		return out;
	};

private:
	virtual void print(std::ostream &out) const = 0;
};

#endif