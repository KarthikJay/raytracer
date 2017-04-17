#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>

#include <stdio.h>
#include <eigen3/Eigen/Dense>

#include "ray.hpp"
#include "light.hpp"
#include "shape.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "stb_image_write.h"

// TODO(kjayakum): Refactor this file... it's bad...
enum class Command : int 
{
	INVALID = 0,
	RENDER = 1,
	SCENEINFO = 2,
	PIXELRAY = 3,
	FIRSTHIT = 4
};

void parse_optional(int argc, char *argv[], std::vector<unsigned int> &optional)
{
	unsigned int temp;
	for (int i = 3; i < argc; i++)
	{
		std::stringstream ss(argv[i]);
		ss >> temp;
		optional.push_back(temp);
	}
}

//TODO(kjayakum): Have filename support being in a folder called resources
Command is_valid_command(int argc, char *argv[])
{
	std::string cur;
	std::ifstream test_file;
	Command type = Command::INVALID;
	// Minimum needed arguments
	if(argc < 3)
	{
		std::cerr << "Usage: " << argv[0] <<  " command filename [width, height] [test_x, test_y]"
					<< std::endl;
		exit(EXIT_FAILURE);
	}

	// TODO(kjayakum): Add further checks to make sure optional arguments are provided for each command
	cur = std::string(argv[1]);
	if(cur == "render")
		type = Command::RENDER;
	else if(cur == "sceneinfo")
		type = Command::SCENEINFO;
	else if(cur == "pixelray")
		type = Command::PIXELRAY;
	else if(cur == "firsthit")
		type = Command::FIRSTHIT;
	if(type == Command::INVALID)
	{
		std::cerr << "Invalid command entered" << std::endl;
		exit(EXIT_FAILURE);
	}

	// TODO(kjayakum): Add support for a resources folder lookup
	cur = std::string(argv[2]);
	test_file.open(cur);
	if(!test_file.good())
	{
		perror(cur.c_str());
		exit(EXIT_FAILURE);
	}
	test_file.close();
	// TODO(kjayakum): Check filename for .pov extension

	return type;
}

// TODO(kjayakum): Parse function assumes Shapes have data on the same line they are declared
void parse_scene(char *filename, Camera &view, std::vector<std::shared_ptr<Light>> &lights,
				 std::vector<std::shared_ptr<Shape>> &objects)
{
	std::ifstream infile;
	std::string line;
	std::size_t pos;
	std::string temp;

	infile.open(filename);
	while(getline(infile, line))
	{
		pos = line.find("//");
		if(pos != std::string::npos)
			line.resize(pos);
		std::replace(line.begin(), line.end(), '{', ' ');
		std::replace(line.begin(), line.end(), '}', ' ');
		std::replace(line.begin(), line.end(), '<', ' ');
		std::replace(line.begin(), line.end(), '>', ' ');
		std::replace(line.begin(), line.end(), ',', ' ');
		// TODO(kjayakum): Assumes light_source data as one line.
		if(line.find("light_source") != std::string::npos)
		{
			std::stringstream ss(line);
			lights.push_back(std::make_shared<Light>());

			ss >> temp;
			ss >> lights.back()->position(0);
			ss >> lights.back()->position(1);
			ss >> lights.back()->position(2);
			ss >> temp;
			// TODO(kjayakum): Hardcoded RGB
			ss >> temp;
			ss >> lights.back()->color(0);
			ss >> lights.back()->color(1);
			ss >> lights.back()->color(2);

			// std::cout << *(lights.back());
		}
		else if(line.find("camera") != std::string::npos)
		{
			// TODO(kjayakum): Will not fully clean up or check lines following camera
			while(line != "}")
			{
				getline(infile, line);
				std::replace(line.begin(), line.end(), '<', ' ');
				std::replace(line.begin(), line.end(), '>', ' ');
				std::replace(line.begin(), line.end(), ',', ' ');
				// TODO(kjayakum): Don't declare temporary objects in loop
				std::stringstream ss(line);
				ss >> temp;
				if(temp == "location")
				{
					ss >> view.position(0);
					ss >> view.position(1);
					ss >> view.position(2);
				}
				
				else if(temp == "up")
				{
					ss >> view.up(0);
					ss >> view.up(1);
					ss >> view.up(2);
				}
				else if(temp == "right")
				{
					ss >> view.right(0);
					ss >> view.right(1);
					ss >> view.right(2);
				}
				else if(temp == "look_at")
				{
					ss >> view.look_at(0);
					ss >> view.look_at(1);
					ss >> view.look_at(2);
				}
			}
		}
		else if(line.find("sphere") != std::string::npos)
		{
			std::stringstream ss(line);
			objects.push_back(std::make_shared<Sphere>());
			Sphere &cur = *(std::static_pointer_cast<Sphere>(objects.back()));

			ss >> temp;
			ss >> cur.center(0);
			ss >> cur.center(1);
			ss >> cur.center(2);
			ss >> cur.radius;

			// TODO(kjayakum): Will not fully clean up or check lines following camera
			// TODO(kjayakum): Look into stringstream ignore
			while(line != "}")
			{
				getline(infile, line);
				std::replace(line.begin(), line.end(), '{', ' ');
				std::replace(line.begin(), line.end(), '<', ' ');
				std::replace(line.begin(), line.end(), '>', ' ');
				std::replace(line.begin(), line.end(), ',', ' ');
				// TODO(kjayakum): Don't declare temporary objects in loop
				std::stringstream ss2(line);
				ss2 >> temp;
				if(temp == "pigment")
				{
					ss2 >> temp;
					ss2 >> temp;
					ss2 >> cur.color(0);
					ss2 >> cur.color(1);
					ss2 >> cur.color(2);
				}
				else if(temp == "finish")
				{
					//TODO(kjayakum): Figure out better way to parse finishes
					for(int i = 0; i < 2; i++)
					{
						ss2 >> temp;
						if(temp == "ambient")
							ss2 >> cur.ambient;
						else if(temp == "diffuse")
							ss2 >> cur.diffuse;
					}
				}
			}
		}
		else if(line.find("plane") != std::string::npos)
		{
			std::stringstream ss(line);
			objects.push_back(std::make_shared<Plane>());
			Plane &cur = *(std::static_pointer_cast<Plane>(objects.back()));

			ss >> temp;
			ss >> cur.normal(0);
			ss >> cur.normal(1);
			ss >> cur.normal(2);
			ss >> cur.distance;

			// TODO(kjayakum): Will not fully clean up or check lines following camera
			// TODO(kjayakum): Look into stringstream ignore
			while(line != "}")
			{
				getline(infile, line);
				std::replace(line.begin(), line.end(), '{', ' ');
				std::replace(line.begin(), line.end(), '<', ' ');
				std::replace(line.begin(), line.end(), '>', ' ');
				std::replace(line.begin(), line.end(), ',', ' ');
				// TODO(kjayakum): Don't declare temporary objects in loop
				std::stringstream ss2(line);
				ss2 >> temp;
				if(temp == "pigment")
				{
					ss2 >> temp;
					ss2 >> temp;
					ss2 >> cur.color(0);
					ss2 >> cur.color(1);
					ss2 >> cur.color(2);
				}
				else if(temp == "finish")
				{
					//TODO(kjayakum): Figure out better way to parse finishes
					for(int i = 0; i < 2; i++)
					{
						ss2 >> temp;
						if(temp == "ambient")
							ss2 >> cur.ambient;
						else if(temp == "diffuse")
							ss2 >> cur.diffuse;
					}
				}
			}
		}
	}
	infile.close();
}

void print_scene(Camera &view, std::vector<std::shared_ptr<Shape>> &objects,
					std::vector<std::shared_ptr<Light>> &lights)
{
	std::cout << "Camera:" << std::endl << view;
	std::cout << std::endl << "---" << std::endl << std::endl;
	std::cout << lights.size() << " light(s)" << std::endl;
	for(unsigned int i = 0; i < lights.size(); i++)
	{
		std::cout << std::endl << "Light[" << i << "]:" << std::endl;
		std::cout << *lights[i];
	}
	std::cout << std::endl << "---" << std::endl << std::endl;
	std::cout << objects.size() << " object(s)" << std::endl;
	for(unsigned int i = 0; i < objects.size(); i++)
	{
		std::cout << std::endl << "Object[" << i << "]:" << std::endl;
		std::cout << *objects[i];
	}
}

void firsthit(unsigned int width, unsigned int height,
				unsigned int x, unsigned int y,
				Camera &view, std::vector<std::shared_ptr<Shape>> &objects)
{
	double u = -0.5 + ((x + 0.5) / width);
	double v = -0.5 + ((y + 0.5) / height);
	double w = -1;
	double t = std::numeric_limits<double>::max();
	int select = 0;
	bool print = false;
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	Eigen::Vector3d look = view.right.cross(view.up.normalized());
	Eigen::Vector3d dis = ((view.right * u) + (view.up.normalized() * v) + (w * look.normalized())).normalized();
	Ray test(view.position, dis);

	std::cout << "Pixel: [" << x << ", " << y << "] ";
	std::cout << "Ray: {" << test.origin.format(SpaceFormat) << "} -> {";
	std::cout << test.direction.format(SpaceFormat) << "}" << std::endl;

	// Loop through objects checking collision
	for(unsigned int i = 0; i < objects.size(); i++)
	{
		double temp = objects[i]->collision(test);
		if(temp > 0)
		{
			if(temp < t)
			{
				print = true;
				select = i;
				t = temp;
			}
		}
	}
	t = std::round(t);
	if(print)
	{
		std::cout << "T = " << t << std::endl;
		std::cout << "Object Type: ";
		objects[select]->print_type();
		std::cout << std::endl;
		std::cout << "Color: " << objects[select]->color.format(SpaceFormat) << std::endl;
	}
	else
	{
		std::cout << "No Hit" << std::endl;
	}
}

void pixelray(unsigned int width, unsigned int height,
				unsigned int x, unsigned int y, Camera &view)
{
	Ray temp;
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	Eigen::Vector3d look = view.right.cross(view.up.normalized());
	Eigen::Vector3d dist;

	temp.origin = view.position;
	std::cout << "Pixel: [" << x << ", " << y << "] ";
	std::cout << "Ray: {" << temp.origin.format(SpaceFormat) << "} -> {";

	double u = -0.5 + ((x + 0.5) / width);
	double v = -0.5 + ((y + 0.5) / height);
	double w = -1;

	dist = (view.right * u) + (view.up.normalized() * v) + (w * look.normalized());
	temp.direction = dist.normalized();
	std::cout << temp.direction.format(SpaceFormat) << "}" << std::endl;
}

void render(unsigned int width, unsigned int height, Camera &view,
			std::vector<std::shared_ptr<Shape>> &objects)
{
	const int num_channels = 3;
	const std::string filename = "output.png";

	for(unsigned int y = 0; y < height; y++)
	{
		for(unsigned int x = 0; x < width; x++)
		{
			unsigned char red = 0, green = 0, blue = 0;
		}
	}
}

int main(int argc, char *argv[])
{
	std::vector<std::shared_ptr<Shape>> objects;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<unsigned int> options;
	Camera view;

	Command type = is_valid_command(argc, argv);
	parse_scene(argv[2], view, lights, objects);
	parse_optional(argc, argv, options);

	switch(type)
	{
		case Command::RENDER:
			break;
		case Command::FIRSTHIT:
			firsthit(options[0], options[1], options[2], options[3], view, objects);
			break;
		case Command::PIXELRAY:
			pixelray(options[0], options[1], options[2], options[3], view);
			break;
		case Command::SCENEINFO:
			print_scene(view, objects, lights);
			break;
		case Command::INVALID:
			break;
	}
	return 0;
}