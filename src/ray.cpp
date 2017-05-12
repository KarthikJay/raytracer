#include "ray.hpp"

Ray::Ray()
{
	position = Eigen::Vector3d(0.0, 0.0, 0.0);
	direction = Eigen::Vector3d(0.0, 0.0, 0.0);
}

Ray::Ray(Eigen::Vector3d &position, Eigen::Vector3d &direction)
{
	this->position = position;
	this->direction = direction;
}

std::ostream &operator<< (std::ostream &out, const Ray &r)
{
	return out;
}

Eigen::Vector3d Ray::get_point(double time) const
{
	return position + (time * direction);
}