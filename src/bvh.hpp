#ifndef BVH_HPP
#define BVH_HPP

#include <memory>
#include <vector>

#include "shape.hpp"
#include "aabb.hpp"

class BVH
{
public:
	BVH *left;
	BVH *right;
	std::vector<std::shared_ptr<Shape>> shapes;
	AABB bounding_box;
	BVH();
	void build_tree(std::vector<std::shared_ptr<Shape>> &shapes, uint axis);

private:
};

#endif