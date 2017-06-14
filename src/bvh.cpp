#include <algorithm>
#include <cmath>

#include "bvh.hpp"

BVH::BVH()
{
	left = NULL;
	right = NULL;
	shapes = std::vector<std::shared_ptr<Shape>>();
	bounding_box = AABB();
}

bool sort_shape_x(const std::shared_ptr<Shape> &a, const std::shared_ptr<Shape> &b)
{
	return a->get_center()(0) > b->get_center()(0);
}

bool sort_shape_y(const std::shared_ptr<Shape> &a, const std::shared_ptr<Shape> &b)
{
	return a->get_center()(1) > b->get_center()(1);
}

bool sort_shape_z(const std::shared_ptr<Shape> &a, const std::shared_ptr<Shape> &b)
{
	return a->get_center()(2) > b->get_center()(2);
}

// TODO(kjayakum): Use an enum for axis instead?
void sort_shapes_on_axis(std::vector<std::shared_ptr<Shape>> &shapes, uint axis)
{
	switch(axis)
	{
	case 0:
		std::sort(shapes.begin(), shapes.end(), sort_shape_x);
		break;
	case 1:
		std::sort(shapes.begin(), shapes.end(), sort_shape_y);
		break;
	case 2:
		std::sort(shapes.begin(), shapes.end(), sort_shape_z);
		break;
	}
}

// TODO(kjayakum): scan for planes in the shape list, and remove them!
void BVH::build_tree(std::vector<std::shared_ptr<Shape>> &shapes, uint axis)
{
	std::size_t split = std::ceil(shapes.size() / 2);
	std::vector<std::shared_ptr<Shape>> left_half =
		std::vector<std::shared_ptr<Shape>>(shapes.begin(), shapes.begin() + split);
	std::vector<std::shared_ptr<Shape>> right_half =
		std::vector<std::shared_ptr<Shape>>(shapes.begin() + split, shapes.end());
	if(shapes.size() <= 1)
	{
		this->shapes = shapes;
		bounding_box.generateAABB(this->shapes);
		return;
	}

	left = new BVH();
	right = new BVH();

	left->build_tree(left_half, (axis + 1) % 3);
	right->build_tree(right_half, (axis + 1) % 3);
	this->shapes = shapes;
	bounding_box.generateAABB(this->shapes);
}