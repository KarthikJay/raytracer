#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <eigen3/Eigen/Dense>
#include <vector>
#include <string>
#include <iostream>

class Shape
{
public:
	// TODO(kjayakum): Figure out translations/skews and rotations.
	double ambient;
	double diffuse;
	Eigen::Vector3d color;

	friend std::ostream &operator<< (std::ostream &out, const Shape &s)
	{
		s.print(out);
		return out;
	};

private:
	virtual void print(std::ostream &out) const = 0;
};

#endif