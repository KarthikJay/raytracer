#ifndef RAY_HPP
#define RAY_HPP

#include <eigen3/Eigen/Dense>
#include <iostream>

#include "camera.hpp"

class Ray
{
public:
	Eigen::Vector3d origin;
	Eigen::Vector3d direction;

	Ray();
	Ray(const Eigen::Vector3d &origin, const Eigen::Vector3d &direction);
	Eigen::Vector3d get_point(double time);
	Ray transform(Eigen::Matrix4d transform) const;
	friend std::ostream &operator<< (std::ostream &out, const Ray &r);
private:

};

#endif