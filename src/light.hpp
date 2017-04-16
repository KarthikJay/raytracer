#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <eigen3/Eigen/Dense>

// Class for a point light
class Light
{
public:
	Eigen::Vector3d position;
	Eigen::Vector3d color;

	Light();
	Light(Eigen::Vector3d position, Eigen::Vector3d color);

	friend std::ostream &operator<< (std::ostream &out, const Light &l);
private:

};

#endif