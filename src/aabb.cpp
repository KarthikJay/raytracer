#include <algorithm>

#include "aabb.hpp"

AABB::AxisAlignedBoundingBox()
{
	min = Eigen::Vector3d::Zero();
	max = Eigen::Vector3d::Zero();
}

void AABB::generateAABB(const std::vector<std::shared_ptr<Shape>> &shapes)
{
	Eigen::Vector3d temp_max, temp_min;
	double max_x = 0, max_y = 0, max_z = 0, min_x = 0, min_y = 0, min_z = 0;
	for(std::shared_ptr<Shape> itr : shapes)
	{
		temp_max = itr->get_world_max_coord();
		temp_min = itr->get_world_min_coord();
		max_x = std::max(max_x, temp_max(0));
		max_y = std::max(max_y, temp_max(1));
		max_z = std::max(max_z, temp_max(2));
		min_x = std::min(min_x, temp_min(0));
		min_y = std::min(min_y, temp_min(1));
		min_z = std::min(min_z, temp_min(2));
	}
	min << min_x, min_y, min_z;
	max << max_x, max_y, max_z;
}

double AABB::collision(Ray &pixel_ray)
{
	double t1, t2;
	double tmin = std::numeric_limits<double>::min();
	double tmax = std::numeric_limits<double>::max();

	// Iterate over axis
	for(uint i = 0; i < 3; i++)
	{
		if(pixel_ray.direction(i) == 0)
			return 0.0;
		t1 = (min(i) - pixel_ray.origin(i)) / pixel_ray.direction(i);
		t2 = (max(i) - pixel_ray.origin(i)) / pixel_ray.direction(i);
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