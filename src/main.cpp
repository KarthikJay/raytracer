#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <eigen3/Eigen/Dense>

#include "light.hpp"
#include "shape.hpp"
#include "sphere.hpp"
#include "camera.hpp"

enum class Command : int 
{
	INVALID = 0,
	RENDER = 1,
	SCENEINFO = 2,
	PIXELRAY = 3,
	FIRSTHIT = 4
};

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
			// TODO(kjayakum): Figure out why static_cast is invalid
			std::shared_ptr<Sphere> t = std::static_pointer_cast<Sphere>(objects.back());
			Sphere &cur = *t;

			ss >> temp;
			ss >> cur.center(0);
			ss >> cur.center(1);
			ss >> cur.center(2);
			//std::cout << cur << std::endl;
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
			std::cout << "Found a plane" << std::endl;
		}
	}
	infile.close();
}

int main(int argc, char *argv[])
{
	std::vector<std::shared_ptr<Shape>> objects;
	std::vector<std::shared_ptr<Light>> lights;
	Camera view;

	Command type = is_valid_command(argc, argv);
	parse_scene(argv[2], view, lights, objects);
	return 0;
}