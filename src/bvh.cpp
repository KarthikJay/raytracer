#include "bvh.hpp"

BVH::BVH()
{
	left = NULL;
	right = NULL;
	shapes = NULL;
	bounding_box = AABB();
}
/*
void sort_shapes_on_axis(std::vector<std::shared_ptr<Shape>> &shapes, uint axis)
{

}

void BVH::build_tree(std::vector<std::shared_ptr<Shape>> shapes, uint axis)
{
	// TODO(kjayakum): scan for planes in the shape list, and remove them!
	std::vector<std::shared_ptr<Shape>> right;
	std::vector<std::shared_ptr<Shape>> left;
	if(shapes.length() <= 1)
	{
		this->shapes = shapes;
		bounding_box.generateAABB(shapes);
		return;
	}


}
*/