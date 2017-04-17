#include "plane.hpp"

Plane::Plane()
{
	normal = Eigen::Vector3d(0.0, 0.0, 0.0);
	distance = 0.0;

	color = Eigen::Vector3d(0.0, 0.0, 0.0);
	ambient = 0.0;
	diffuse = 0.0;
}

Plane::Plane(Eigen::Vector3d normal, double distance,
				Eigen::Vector3d color, double ambient, double diffuse)
{
	this->normal = normal;
	this->distance = distance;

	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
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
}