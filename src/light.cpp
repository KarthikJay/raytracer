#include "light.hpp"

Light::Light()
{
    position = Eigen::Vector3d(0.0, 0.0, 0.0);
    color = Eigen::Vector3d(0.0, 0.0, 0.0);
}

Light::Light(Eigen::Vector3d position, Eigen::Vector3d color)
{
    this->position = position;
    this->color = color;
}

// TODO(kjayakum): Look into making this an interface for scene object
std::ostream &operator<< (std::ostream &out, const Light &l)
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Location: {" << l.position.format(SpaceFormat) << "}" << std::endl;
	out << "- Color: {" << l.color.format(SpaceFormat) << "}" << std::endl;

	return out;
}