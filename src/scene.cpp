#include <sstream>
#include <string>
#include <algorithm>

#include "scene.hpp"
#include "sphere.hpp"
#include "plane.hpp"

Scene::Scene() {}

void remove_comment(std::string &line)
{
	std::size_t pos = line.find("//");
	if(pos != std::string::npos)
		line.resize(pos);
}

void replace_markers(std::string &line, bool replace_braces = true)
{
	if(replace_braces)
	{
		std::replace(line.begin(), line.end(), '{', ' ');
		std::replace(line.begin(), line.end(), '}', ' ');
	}
	std::replace(line.begin(), line.end(), '<', ' ');
	std::replace(line.begin(), line.end(), '>', ' ');
	std::replace(line.begin(), line.end(), ',', ' ');
}

void read_camera(std::istream &in, Scene &scene)
{
	std::string temp;
	std::string line;
	while(line != "}")
	{
		getline(in, line);
		replace_markers(line, false);
		std::stringstream ss(line);
		ss >> temp;
		if(temp == "location")
		{
			ss >> scene.view.position(0);
			ss >> scene.view.position(1);
			ss >> scene.view.position(1);
		}
		else if(temp == "up")
		{
			ss >> scene.view.up(0);
			ss >> scene.view.up(1);
			ss >> scene.view.up(2);
		}
		else if(temp == "right")
		{
			ss >> scene.view.right(0);
			ss >> scene.view.right(1);
			ss >> scene.view.right(2);
		}
		else if(temp == "look_at")
		{
			ss >> scene.view.look_at(0);
			ss >> scene.view.look_at(1);
			ss >> scene.view.look_at(2);
		}
	}
}

void read_pigment(std::stringstream &itr, Shape &shape)
{
	std::string temp;

	// Consume "color"
	itr >> temp;
	// Consume "rgb"
	itr >> temp;

	itr >> shape.color(0);
	itr >> shape.color(1);
	itr >> shape.color(2);
}

void read_finish(std::stringstream &itr, Shape &shape)
{
	std::string temp;
	while(temp != "}")
	{
		itr >> temp;
		if(temp == "ambient")
			itr >> shape.ambient;
		else if(temp == "diffuse")
			itr >> shape.diffuse;
		else if(temp == "specular")
			itr >> shape.specular;
		else if(temp == "roughness")
			itr >> shape.roughness;
	}
}

void read_shape_properties(std::string input, Shape &shape)
{
	std::string temp;
	std::replace(input.begin(), input.end(), '{', ' ');
	std::stringstream ss(input);
	ss >> temp;

	if(temp == "pigment")
		read_pigment(ss, shape);
	else if(temp == "finish")
		read_finish(ss, shape);
}

// Light Sources are assumed to be single line
void read_light(std::string &line, Scene &scene)
{
	Light cur_light;
	std::string temp;
	std::stringstream ss(line);

	// Consumes "light_source"
	ss >> temp;

	ss >> cur_light.position(0);
	ss >> cur_light.position(1);
	ss >> cur_light.position(2);

	// Consumes "color"
	ss >> temp;
	// Consumes "rgb"
	ss >> temp;

	ss >> cur_light.color(0);
	ss >> cur_light.color(1);
	ss >> cur_light.color(2);

	scene.lights.push_back(cur_light);
}

void read_spheres(std::istream &in, std::string line, Scene &scene)
{
	std::shared_ptr<Sphere> cur_sphere = std::make_shared<Sphere>();
	std::string temp;
	std::string property_line;
	std::stringstream ss(line);

	// Consume "sphere"
	ss >> temp;

	ss >> cur_sphere->center(0);
	ss >> cur_sphere->center(1);
	ss >> cur_sphere->center(2);
	ss >> cur_sphere->radius;
	while(property_line != "}")
	{
		getline(in, property_line);
		replace_markers(property_line, false);
		read_shape_properties(property_line, *cur_sphere);
	}

	scene.shapes.push_back(cur_sphere);
}

void read_planes(std::istream &in, std::string line, Scene &scene)
{
	std::shared_ptr<Plane> cur_plane = std::make_shared<Plane>();
	std::string temp;
	std::string property_line;
	std::stringstream ss(line);

	// Consume "plane"
	ss >> temp;

	ss >> cur_plane->normal(0);
	ss >> cur_plane->normal(1);
	ss >> cur_plane->normal(2);
	ss >> cur_plane->distance;
	while(property_line != "}")
	{
		getline(in, property_line);
		replace_markers(property_line, false);
		read_shape_properties(property_line, *cur_plane);
	}

	scene.shapes.push_back(cur_plane);
}

std::istream &operator>> (std::istream &in, Scene &scene)
{
	std::string line;
	while(getline(in, line))
	{
		remove_comment(line);
		replace_markers(line);
		if(line.find("camera") != std::string::npos)
			read_camera(in, scene);
		else if(line.find("light_source") != std::string::npos)
			read_light(line, scene);
		else if(line.find("sphere") != std::string::npos)
			read_spheres(in, line, scene);
		else if(line.find("plane") != std::string::npos)
			read_planes(in, line, scene);
	}
	return in;
}

void Scene::print_visual(std::ostream &out) const
{
	out << "Camera:" << std::endl << view;
	out << std::endl << "---" << std::endl << std::endl;
	out << lights.size() << " light(s)" << std::endl;
	for(unsigned int i = 0; i < lights.size(); i++)
	{
		out << std::endl << "Light[" << i << "]:" << std::endl;
		out << lights[i];
	}
	out << std::endl << "---" << std::endl << std::endl;
	out << shapes.size() << " object(s)" << std::endl;
	for(unsigned int i = 0; i < shapes.size(); i++)
	{
		out << std::endl << "Object[" << i << "]:" << std::endl;
		out << *shapes[i];
	}
}

// TODO(kjayakum): Implement writing scene to pov input file
std::ostream &operator<< (std::ostream &out, const Scene &scene)
{
	scene.print_visual(out);
	return out;
}