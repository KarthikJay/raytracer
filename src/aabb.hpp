#ifndef AABB_HPP
#define AABB_HPP

#include <eigen3/Eigen/Dense>
#include <vector>
#include <memory>

#include "shape.hpp"

class AxisAlignedBoundingBox
{
public:
	Eigen::Vector3d min;
	Eigen::Vector3d max;
	Eigen::Vector3d center;
	AxisAlignedBoundingBox();
	void generateAABB(const std::vector<std::shared_ptr<Shape>> &shapes);
	double collision(Ray &pixel_ray);

private:
};
typedef AxisAlignedBoundingBox AABB;

#endif