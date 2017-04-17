#include "ray.hpp"

Ray::Ray()
{
	origin = Eigen::Vector3d(0.0, 0.0, 0.0);
	direction = Eigen::Vector3d(0.0, 0.0, 0.0);
}

Ray::Ray(Eigen::Vector3d &origin, Eigen::Vector3d &direction)
{
	this->origin = origin;
	this->direction = direction;
}

std::ostream &operator<< (std::ostream &out, const Ray &r)
{
	return out;
}