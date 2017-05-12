#ifndef RAY_HPP
#define RAY_HPP

#include <eigen3/Eigen/Dense>
#include <iostream>

#include "camera.hpp"

class Ray
{
public:
	Eigen::Vector3d position;
	Eigen::Vector3d direction;

	Ray();
	Ray(Eigen::Vector3d &position, Eigen::Vector3d &direction);
	Eigen::Vector3d get_point(double time) const;
	friend std::ostream &operator<< (std::ostream &out, const Ray &r);
private:

};

#endif