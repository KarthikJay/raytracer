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

bool epsilon_compare(double a, double b, double const epsilon = std::numeric_limits<double>::epsilon())
{
	return std::abs(a - b) < epsilon;
}

// TODO(kjayakum): Fix normals!
Eigen::Vector3d Box::get_normal(Eigen::Vector3d point)
{
	Eigen::Vector3d normal = Eigen::Vector3d::Zero();

	if(epsilon_compare(point(0), min(0)))
	{
		normal << -1, 0, 0;
	}
	else if(epsilon_compare(point(0), max(0)))
	{
		normal << 1, 0, 0;
	}
	else if(epsilon_compare(point(1), min(1)))
	{
		normal << 0, -1, 0;
	}
	else if(epsilon_compare(point(2), max(2)))
	{
		normal << 0, 1, 0;
	}
	else if(epsilon_compare(point(2), min(2)))
	{
		normal << 0, 0, -1;
	}
	else if(epsilon_compare(point(2), max(2)))
	{
		normal << 0, 0, 1;
	}
	normal << (inverse_transform.transpose()
			* (Eigen::Vector4d() << normal.head<3>(), 0).finished()).head<3>();

	return normal.normalized();
}