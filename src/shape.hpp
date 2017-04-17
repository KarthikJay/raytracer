#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <eigen3/Eigen/Dense>
#include <vector>
#include <string>
#include <iostream>

#include "ray.hpp"

class Shape
{
public:
	// TODO(kjayakum): Figure out translations/skews and rotations.
	double ambient;
	double diffuse;
	Eigen::Vector3d color;

	// TODO(kjayakum): Figure out neater implementation using stream manipulators.
	virtual void print_type() = 0;
	virtual double collision(Ray &r) = 0;
	friend std::ostream &operator<< (std::ostream &out, const Shape &s)
	{
		s.print(out);
		return out;
	};

private:
	virtual void print(std::ostream &out) const = 0;
};

#endif