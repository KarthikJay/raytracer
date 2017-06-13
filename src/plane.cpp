#include <algorithm>

#include "plane.hpp"

Plane::Plane()
{
	normal = Eigen::Vector3d(0.0, 0.0, 0.0);
	distance = 0.0;
	set_material_defaults();
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
	this->print_material(out);
}

void Plane::print_type(std::ostream &out) const
{
	out << "Plane";
}

double Plane::collision(Ray &r)
{
	double time = (this->distance - (r.origin.dot(this->normal.normalized())))
				/ r.direction.dot(this->normal.normalized());
	time = time < 0.0 ? 0.0 : time;
	return time;
}

Eigen::Vector3d Plane::get_normal(Eigen::Vector3d point)
{
	return normal;
}

Eigen::Vector3d Plane::get_world_max_coord()
{
	return Eigen::Vector3d::Zero();
}
Eigen::Vector3d Plane::get_world_min_coord()
{
	return Eigen::Vector3d::Zero();
}

Eigen::Vector3d Plane::get_center()
{
	return Eigen::Vector3d::Zero();
}