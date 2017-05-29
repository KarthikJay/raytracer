#include "shape.hpp"

void Shape::print_material(std::ostream &out) const
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

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

void Shape::set_material_defaults()
{
	color = Eigen::Vector3d(0.0, 0.0, 0.0);
	inverse_transform = Eigen::Matrix4d::Identity();
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