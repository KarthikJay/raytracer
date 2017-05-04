#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <memory>
#include <iostream>
#include <eigen3/Eigen/Dense>

#include "shape.hpp"
#include "camera.hpp"
#include "light.hpp"

class Scene
{
public:
	std::vector<std::shared_ptr<Shape>> shapes;
	std::vector<Light> lights;
	Camera view;

	Scene();
	// Print out human readable format
	void print_visual(std::ostream &out) const;
	friend std::istream &operator>> (std::istream &in, Scene &scene);
	friend std::ostream &operator<< (std::ostream &out, const Scene &scene);

private:

};

#endif