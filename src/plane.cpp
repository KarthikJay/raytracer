#include <algorithm>

#include "plane.hpp"

Plane::Plane()
{
	normal = Eigen::Vector3d(0.0, 0.0, 0.0);
	distance = 0.0;

	color = Eigen::Vector3d(0.0, 0.0, 0.0);
	ambient = 0.0;
	diffuse = 0.0;
	specular = 0.0;
	roughness = 0.5;
	metallic = 0.0;
	ior = 1.0;
}

Plane::Plane(Eigen::Vector3d normal, double distance,
				Eigen::Vector3d color, double ambient, double diffuse,
				double specular, double roughness)
{
	this->normal = normal;
	this->distance = distance;

	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->roughness = roughness;
}

void Plane::print(std::ostream &out) const
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Type: Plane" << std::endl;
	out << "- Normal: {" << this->normal.format(SpaceFormat) << "}" << std::endl;
	out << "- Distance: " << this->distance << std::endl;
	out << "- Color: {" << this->color.format(SpaceFormat) << "}" << std::endl;
	out << "- Material:" << std::endl;
	out << "  - Ambient: " << this->ambient << std::endl;
	out << "  - Diffuse: " << this->diffuse << std::endl;
	out << "  - Specular: " << this->specular << std::endl;
	out << "  - Roughness: " << this->roughness << std::endl;
	out << "  - Metallic: " << this->metallic << std::endl;
	out << "  - Index of Refraction: " << this->ior << std::endl;
}

void Plane::print_type(std::ostream &out) const
{
	out << "Plane";
}

double Plane::collision(Ray &r)
{
	double T = (this->distance - (r.origin.dot(this->normal.normalized())))
				/ r.direction.dot(this->normal.normalized());
	T = T < 0.0 ? 0.0 : T;
	return T;
}

Eigen::Vector3d Plane::get_normal(Eigen::Vector3d point)
{
	return normal;
}