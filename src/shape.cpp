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