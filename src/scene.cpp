#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>

#include "scene.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "box.hpp"

Scene::Scene() {}

void Scene::set_scene_dimensions(uint width, uint height)
{
	this->width = width;
	this->height = height;
}

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

// TODO(kjayakum): Rafactor the below function better
void read_camera(std::istream &in, Scene &scene, std::string line)
{
	std::string temp;
	if(line != "camera")
	{
		std::stringstream ss(line);
		ss >> temp;
		ss >> temp;

		if(temp == "location")
		{
			ss >> scene.view.position(0);
			ss >> scene.view.position(1);
			ss >> scene.view.position(2);
		}
	}
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
			ss >> scene.view.position(2);
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
	// Consume "rgb/f"
	itr >> temp;

	itr >> shape.color(0);
	itr >> shape.color(1);
	itr >> shape.color(2);
	if(temp == "rgbf")
		itr >> shape.filter;
}

void read_finish(std::stringstream &itr, Shape &shape)
{
	std::string temp;
	while(temp != "}" && temp != "}}")
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
		else if(temp == "reflection")
			itr >> shape.reflection;
		else if(temp == "refraction")
			itr >> shape.refraction;
		else if(temp == "metallic")
			itr >> shape.metallic;
		else if(temp == "ior")
			itr >> shape.ior;
	}
}

Eigen::Affine3d create_rotation_matrix(double x, double y, double z)
{
	Eigen::Affine3d rx = 
		Eigen::Affine3d(Eigen::AngleAxisd(x, Eigen::Vector3d(1, 0, 0)));
	Eigen::Affine3d ry =
		Eigen::Affine3d(Eigen::AngleAxisd(y, Eigen::Vector3d(0, 1, 0)));
	Eigen::Affine3d rz =
		Eigen::Affine3d(Eigen::AngleAxisd(z, Eigen::Vector3d(0, 0, 1)));

	return rz * ry * rx;
}

void read_scale(std::stringstream &itr, Shape &shape)
{
	double x, y, z;
	itr >> x;
	itr >> y;
	itr >> z;
	Eigen::Matrix4d scale = Eigen::Matrix4d::Identity();
	scale(0, 0) = x;
	scale(1, 1) = y;
	scale(2, 2) = z;
	shape.inverse_transform *= scale;
}

void read_rotation(std::stringstream &itr, Shape &shape)
{
	double x, y, z;
	itr >> x;
	itr >> y;
	itr >> z;
	x = x * M_PI / 180.0;
	y = y * M_PI / 180.0;
	z = z * M_PI / 180.0;
	Eigen::Affine3d rotation = create_rotation_matrix(x, y, z);
	shape.inverse_transform *= rotation.matrix();

	// TODO(kjayakum): Ask professor why I need this "fix"?
	x = std::abs(shape.inverse_transform(0,1));
	y = std::abs(shape.inverse_transform(1,0));
	shape.inverse_transform(1, 0) = std::signbit(shape.inverse_transform(1, 0)) ? -x : x;
	shape.inverse_transform(0, 1) = std::signbit(shape.inverse_transform(0, 1)) ? -y : y;
}

void read_translation(std::stringstream &itr, Shape &shape)
{
	// TODO(kjayakum): Ask professor about this scaling?
	//Eigen::Vector3d translate;
	// X translation
	itr >> shape.inverse_transform(0, 3);
	// Y translation
	itr >> shape.inverse_transform(1, 3);
	// Z translation
	itr >> shape.inverse_transform(2, 3);
	//Eigen::Affine3d translation(Eigen::Translation3d(translate.head<3>()));
	//shape.inverse_transform *= translation.matrix();
}

void read_shape_properties(std::string &input, Shape &shape)
{
	std::size_t second_brace = input.find("}");
	second_brace = input.find("}", second_brace + 1);
	std::string temp;
	std::replace(input.begin(), input.end(), '{', ' ');
	std::stringstream ss(input);
	ss >> temp;
	if(temp == "pigment")
		read_pigment(ss, shape);
	else if(temp == "finish")
		read_finish(ss, shape);
	else if(temp == "scale")
	{
		read_scale(ss, shape);
	}
	else if(temp == "rotate")
	{
		read_rotation(ss, shape);
	}
	else if(temp == "translate")
	{
		read_translation(ss, shape);
	}

	if(second_brace != std::string::npos)
		input.erase(input.begin(), input.begin() + second_brace);
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
	cur_sphere->inverse_transform = cur_sphere->inverse_transform.inverse().eval();
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
	cur_plane->inverse_transform = cur_plane->inverse_transform.inverse().eval();
	scene.shapes.push_back(cur_plane);
}

void read_triangles(std::istream &in, std::string line, Scene &scene)
{
	std::shared_ptr<Triangle> cur_triangle = std::make_shared<Triangle>();
	std::string temp;
	std::string property_line;
	std::stringstream ss(line);

	// Consume "triangle"
	ss >> temp;
	for(uint i = 0; i < 3; i++)
	{
		getline(in, property_line);
		replace_markers(property_line);
		std::stringstream ss2(property_line);
		ss2 >> (cur_triangle->points[i])(0);
		ss2 >> (cur_triangle->points[i])(1);
		ss2 >> (cur_triangle->points[i])(2);
	}
	while(property_line != "}")
	{
		getline(in, property_line);
		replace_markers(property_line, false);
		read_shape_properties(property_line, *cur_triangle);
	}
	cur_triangle->inverse_transform = cur_triangle->inverse_transform.inverse().eval();
	scene.shapes.push_back(cur_triangle);
}

void read_boxes(std::istream &in, std::string line, Scene &scene)
{
	std::shared_ptr<Box> cur_box = std::make_shared<Box>();
	std::string temp;
	std::string property_line;
	std::stringstream ss(line);

	// Consume "box"
	ss >> temp;

	ss >> cur_box->min(0);
	ss >> cur_box->min(1);
	ss >> cur_box->min(2);
	ss >> cur_box->max(0);
	ss >> cur_box->max(1);
	ss >> cur_box->max(2);
	while(property_line != "}")
	{
		getline(in, property_line);
		replace_markers(property_line, false);
		read_shape_properties(property_line, *cur_box);
	}
	cur_box->inverse_transform = cur_box->inverse_transform.inverse().eval();
	scene.shapes.push_back(cur_box);
}

std::istream &operator>> (std::istream &in, Scene &scene)
{
	std::string line;
	while(getline(in, line))
	{
		remove_comment(line);
		replace_markers(line);
		if(line.find("camera") != std::string::npos)
			read_camera(in, scene, line);
		else if(line.find("light_source") != std::string::npos)
			read_light(line, scene);
		else if(line.find("sphere") != std::string::npos)
			read_spheres(in, line, scene);
		else if(line.find("plane") != std::string::npos)
			read_planes(in, line, scene);
		else if(line.find("triangle") != std::string::npos)
			read_triangles(in, line, scene);
		else if(line.find("box") != std::string::npos)
			read_boxes(in, line, scene);
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