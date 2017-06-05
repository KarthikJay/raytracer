#include <math.h>
#include <algorithm>

#include "sphere.hpp"

Sphere::Sphere()
{
	center = Eigen::Vector3d(0.0, 0.0, 0.0);
	radius = 0.0;
	set_material_defaults();
}

Sphere::Sphere(Eigen::Vector3d center, double radius,
			Eigen::Vector3d color, double ambient, double diffuse,
			double specular, double roughness)
{
	this->center = center;
	this->radius = radius;

	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->roughness = roughness;
}

void Sphere::print(std::ostream &out) const
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Type: Sphere" << std::endl;
	out << "- Center: {" << this->center.format(SpaceFormat) << "}" << std::endl;
	out << "- Radius: " << this->radius << std::endl;
	this->print_material(out);
}

double Sphere::collision(Ray &r)
{
	Ray object_ray = r.transform(inverse_transform);
	double t1, t2;
	double T = 0.0;
	double A = object_ray.direction.dot(object_ray.direction);
	double B = (2 * object_ray.direction).dot(object_ray.origin - this->center);
	double C = (object_ray.origin - this->center).dot(object_ray.origin - this->center) - (this->radius * this->radius);

	// Positive
	t1 = (-B + std::sqrt(std::pow(B, 2) - (4 * A * C))) / (2 * A);
	// Negative
	t2 = (-B - std::sqrt(std::pow(B, 2) - (4 * A * C))) / (2 * A);

	T = std::min(t1, t2);
	// Remove negative time
	T = std::max(0.0, T);
	return T;
}

void Sphere::print_type(std::ostream &out) const
{
	out << "Sphere";
}

Eigen::Vector3d Sphere::get_normal(Eigen::Vector3d point)
{
	Eigen::Vector3d object_point = point;
	/*
	object_point << (inverse_transform
					* (Eigen::Vector4d() << point.head<3>(), 1).finished()).head<3>();
	*/
	Eigen::Vector3d normal = (object_point - center).normalized();
	normal << (inverse_transform.transpose()
			* (Eigen::Vector4d() << normal.head<3>(), 0).finished()).head<3>();
	return normal.normalized();
}

Eigen::Vector3d Sphere::get_world_max_coord()
{
	Eigen::Vector3d world_max;
	world_max(0) = center(0) + radius;
	world_max(1) = center(1) + radius;
	world_max(2) = center(2) + radius;
	world_max << (transform * (Eigen::Vector4d() << world_max.head<3>(), 1).finished()).head<3>();
	return world_max;
}

Eigen::Vector3d Sphere::get_world_min_coord()
{
	Eigen::Vector3d world_min;
	world_min(0) = center(0) - radius;
	world_min(1) = center(1) - radius;
	world_min(2) = center(2) - radius;
	world_min << (transform * (Eigen::Vector4d() << world_min.head<3>(), 1).finished()).head<3>();
	return world_min;
}