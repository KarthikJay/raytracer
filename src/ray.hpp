#ifndef RAY_HPP
#define RAY_HPP

#include <eigen3/Eigen/Dense>
#include <iostream>

class Ray
{
public:
	Eigen::Vector3d origin;
	Eigen::Vector3d direction;

	Ray();
	Ray(Eigen::Vector3d origin, Eigen::Vector3d direction);
	friend std::ostream &operator<< (std::ostream &out, const Ray &r);
private:

};

#endif