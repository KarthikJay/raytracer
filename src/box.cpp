#include <math.h>
#include <algorithm>

#include "box.hpp"

Box::Box()
{
	min = Eigen::Vector3d(0.0, 0.0, 0.0);
	max = Eigen::Vector3d(0.0, 0.0, 0.0);
	set_material_defaults();
}

void Box::print(std::ostream &out) const
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Type: Box" << std::endl;
	out << "- Min: {" << this->min.format(SpaceFormat) << "}" << std::endl;
	out << "- Max: {" << this->max.format(SpaceFormat) << "}" << std::endl;
	this->print_material(out);
}

double Box::collision(Ray &r)
{
	Ray object_ray = r.transform(inverse_transform);
	double t1, t2;
	double tmin = std::numeric_limits<double>::min();
	double tmax = std::numeric_limits<double>::max();

	// Iterate over axis
	for(uint i = 0; i < 3; i++)
	{
		if(object_ray.direction(i) == 0)
			return 0.0;
		t1 = (min(i) - object_ray.origin(i)) / object_ray.direction(i);
		t2 = (max(i) - object_ray.origin(i)) / object_ray.direction(i);
		if(t1 > t2)
			std::swap(t1, t2);
		tmin = t1 > tmin ? t1 : tmin;
		tmax = t2 < tmax ? t2 : tmax;
	}
	if(tmin > tmax)
		return 0.0;
	if(tmax < 0)
		return 0.0;
	tmin = tmin > 0 ? tmin : tmax;
	return tmin;
}

void Box::print_type(std::ostream &out) const
{
	out << "Box";
}

bool epsilon_compare(double a, double b, double const epsilon = 0.0001)
{
	return std::abs(a - b) < epsilon;
}

// TODO(kjayakum): Use asserts to guard against divide by zeroes!
Eigen::Vector3d Box::get_normal(Eigen::Vector3d point)
{
	Eigen::Vector3d normal = Eigen::Vector3d::Zero();
	Eigen::Vector3d object_point = point;
	object_point << (inverse_transform
					* (Eigen::Vector4d() << point.head<3>(), 1).finished()).head<3>();

	if(epsilon_compare(object_point(0), min(0)))
	{
		normal << -1, 0, 0;
	}
	else if(epsilon_compare(object_point(0), max(0)))
	{
		normal << 1, 0, 0;
	}
	else if(epsilon_compare(object_point(1), min(1)))
	{
		normal << 0, -1, 0;
	}
	else if(epsilon_compare(object_point(1), max(1)))
	{
		normal << 0, 1, 0;
	}
	else if(epsilon_compare(object_point(2), min(2)))
	{
		normal << 0, 0, -1;
	}
	else if(epsilon_compare(object_point(2), max(2)))
	{
		normal << 0, 0, 1;
	}
	normal << (inverse_transform.transpose()
			* (Eigen::Vector4d() << normal.head<3>(), 0).finished()).head<3>();

	return normal.normalized();
}

Eigen::Vector3d Box::get_world_max_coord()
{
	Eigen::Vector3d world_max = max;
	world_max << (transform * (Eigen::Vector4d() << world_max.head<3>(), 1).finished()).head<3>();
	return world_max;
}

Eigen::Vector3d Box::get_world_min_coord()
{
	Eigen::Vector3d world_min = min;
	world_min << (transform * (Eigen::Vector4d() << world_min.head<3>(), 1).finished()).head<3>();
	return world_min;
}