#include "ray.hpp"

Ray::Ray()
{
	origin = Eigen::Vector3d(0.0, 0.0, 0.0);
	direction = Eigen::Vector3d(0.0, 0.0, 0.0);
}

Ray::Ray(const Eigen::Vector3d &origin, const Eigen::Vector3d &direction)
{
	this->origin = origin;
	this->direction = direction;
}

std::ostream &operator<< (std::ostream &out, const Ray &r)
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	out << "Ray: {" << r.origin.format(SpaceFormat) << "} -> {";
	out << r.direction.format(SpaceFormat) << "}" << std::endl;
	return out;
}

Eigen::Vector3d Ray::get_point(double time)
{
	return origin + (time * direction);
}

Ray Ray::transform(Eigen::Matrix4d transform) const
{
	Eigen::Vector4d transform_pos;
	transform_pos << origin.head<3>(), 1;
	Eigen::Vector4d transform_dir;
	transform_dir << direction.head<3>(), 0;
	transform_pos = transform * transform_pos;
	transform_dir = transform * transform_dir;
	return Ray((Eigen::Vector3d() << transform_pos.head<3>()).finished(),
				(Eigen::Vector3d() << transform_dir.head<3>()).finished());
}