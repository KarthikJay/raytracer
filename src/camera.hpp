#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <eigen3/Eigen/Dense>
#include <iostream>

// Class for a Camera Object
class Camera
{
public:
	Eigen::Vector3d position;
	Eigen::Vector3d up;
	Eigen::Vector3d right;
	Eigen::Vector3d look_at;

	Camera();
	Camera(Eigen::Vector3d position, Eigen::Vector3d up,
			Eigen::Vector3d right, Eigen::Vector3d look_at);

	friend std::ostream &operator<< (std::ostream &out, const Camera &c);
private:

};

#endif