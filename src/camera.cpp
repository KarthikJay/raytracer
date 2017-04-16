#include "camera.hpp"

Camera::Camera()
{
	position = Eigen::Vector3d(0.0, 0.0, 0.0);
	up = Eigen::Vector3d(0.0, 0.0, 0.0);
	right = Eigen::Vector3d(0.0, 0.0, 0.0);
	look_at = Eigen::Vector3d(0.0, 0.0, 0.0);
}

Camera::Camera(Eigen::Vector3d position, Eigen::Vector3d up,
				Eigen::Vector3d right, Eigen::Vector3d look_at)
{
	this->position = position;
	this->up = up;
	this->right = right;
	this->look_at = look_at;
}

// TODO(kjayakum): Look into making this an interface for scene object
std::ostream &operator<< (std::ostream &out, const Camera &c)
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Location: {" << c.position.format(SpaceFormat) << "}" << std::endl;
	out << "- Up: {" << c.up.format(SpaceFormat) << "}" << std::endl;
	out << "- Right: {" << c.right.format(SpaceFormat) << "}" << std::endl;
	out << "- Look at: {" << c.look_at.format(SpaceFormat) << "}" << std::endl;

	return out;
}