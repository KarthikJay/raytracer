#include "bvh.hpp"

BVH::BVH()
{
	left = NULL;
	right = NULL;
	shapes = NULL;
	bounding_box = AABB();
}
/*
sort_shapes_on_axis(std::vector<std::shared_ptr<Shape>> &shapes, uint axis)
{

}

void BVH::build_tree(const std::vector<std::shared_ptr<Shape>> &shapes, uint axis)
{
	std::vector<std::shared_ptr<Shape>>
	if(shapes.length() <= 1)
	{
		this->shapes = shapes;
		bounding_box.generateAABB(shapes);
		return;
	}


}
*/