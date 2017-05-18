#include <math.h>
#include <algorithm>

#include "sphere.hpp"

Sphere::Sphere()
{
	center = Eigen::Vector3d(0.0, 0.0, 0.0);
	radius = 0.0;

	color = Eigen::Vector3d(0.0, 0.0, 0.0); 
	ambient = 0.0;
	diffuse = 0.0;
	specular = 0.0;
	roughness = 0.5;
	reflection = 0.0;
	refraction = 0.0;
	metallic = 0.0;
	ior = 1.0;
	filter = 0.0;
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
	out << "- Color: {" << this->color.format(SpaceFormat) << "}" << std::endl;
	out << "- Material:" << std::endl;
	out << "  - Ambient: " << this->ambient << std::endl;
	out << "  - Diffuse: " << this->diffuse << std::endl;
	out << "  - Specular: " << this->specular << std::endl;
	out << "  - Roughness: " << this->roughness << std::endl;
	out << "  - Reflection: " << this->reflection << std::endl;
	out << "  - Refraction: " << this->refraction << std::endl;
	out << "  - Metallic: " << this->metallic << std::endl;
	out << "  - Index of Refraction: " << this->ior << std::endl;
	out << "  - Filter: " << this->filter << std::endl;
}

double Sphere::collision(Ray &r)
{
	double t1, t2;
	double T = 0.0;
	double A = r.direction.dot(r.direction);
	double B = (2 * r.direction).dot(r.origin - this->center);
	double C = (r.origin - this->center).dot(r.origin - this->center) - (this->radius * this->radius);

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
	return point - center;
}