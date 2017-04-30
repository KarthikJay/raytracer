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
#include <eigen3/Eigen/Dense>

#include "ray.hpp"
#include "light.hpp"
#include "shape.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "utility.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template <typename T>
T clamp(const T n, const T lower, const T upper)
{
	return std::max(lower, std::min(n, upper));
}

// TODO(kjayakum): Parse function assumes Shapes have data on the same line they are declared
// TODO(kjayakum): Figure out more concise way of writing this function and it's arguments
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
	if(print)
	{
		std::cout << "T = " << std::setprecision(4) <<  t << std::endl;
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

bool cast_shadow_ray(Ray &test, std::vector<std::shared_ptr<Shape>> &objects,
					 Light &cur_light)
{
	bool is_shadowed = false;
	double t = std::numeric_limits<double>::max();

	for(unsigned int i = 0; i < objects.size(); i++)
	{
		double temp = objects[i]->collision(test);
		if(temp > 0)
		{
			if(temp < t)
			{
				t = temp;
				Eigen::Vector3d hit_point = test.get_point(t);
				double s = (hit_point - test.origin).norm();
				double check = (cur_light.position - test.origin).norm();
				if(s < check)
				{
					is_shadowed = true;
					break;
				}
			}
		}
	}
	return is_shadowed;
}

void pixelcolor(unsigned int width, unsigned int height,
				unsigned int x, unsigned int y,
				Camera &view, std::vector<std::shared_ptr<Shape>> &objects,
				std::vector<std::shared_ptr<Light>> &lights, bool use_alt)
{
	Eigen::Vector3d brdf_color;
	double u = -0.5 + ((x + 0.5) / width);
	double v = -0.5 + ((y + 0.5) / height);
	double w = -1;
	double t = std::numeric_limits<double>::max();
	int select = 0;
	bool print = false;
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	Eigen::IOFormat ParenthesisFormat(4, Eigen::DontAlignCols, "", ", ", "", "", "(", ")");
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

	if(print)
	{
		std::cout << "T = " << std::setprecision(4) << t << std::endl;
		std::cout << "Object Type: ";
		objects[select]->print_type();
		std::cout << std::endl;
		std::cout << "BRDF: ";
		if(!use_alt)
			std::cout << "Blinn-Phong";
		else
			std::cout << "Alternate";
		std::cout << std::endl;
		brdf_color = objects[select]->ambient * objects[select]->color;
		for(unsigned int i = 0; i < lights.size(); i++)
		{
			Eigen::Vector3d v_vec = -dis;
			v_vec.normalize();
			Eigen::Vector3d l_vec = lights[i]->position - test.get_point(t);
			l_vec.normalize();
			Eigen::Vector3d offset = test.get_point(t) + Eigen::Vector3d(0.001, 0.001, 0.001);
			Ray shadow(offset, l_vec);
			Eigen::Vector3d n_vec = objects[select]->get_normal(test.get_point(t));
			n_vec.normalize();
			Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();
			//Eigen::Vector3d r_vec = l_vec - (2 * (l_vec.dot(n_vec)) * n_vec);
			//r_vec.normalize();
			bool is_shadowed = cast_shadow_ray(shadow, objects, *lights[i]);
			if(!is_shadowed)
			{
				Eigen::Vector3d kd = objects[select]->diffuse * objects[select]->color;
				kd(0) *= lights[i]->color(0);
				kd(1) *= lights[i]->color(1);
				kd(2) *= lights[i]->color(2);
				double stuff = n_vec.dot(l_vec);
				brdf_color += kd * stuff;
				Eigen::Vector3d ks = objects[select]->specular * objects[select]->color;
				ks(0) *= lights[i]->color(0);
				ks(1) *= lights[i]->color(1);
				ks(2) *= lights[i]->color(2);
				double stuff2 = std::pow(h_vec.dot(n_vec), objects[select]->roughness);
				brdf_color += ks * stuff2;
			}
		}
		brdf_color *= 255;
		brdf_color(0) = std::round(brdf_color(0));
		brdf_color(1) = std::round(brdf_color(1));
		brdf_color(2) = std::round(brdf_color(2));
		std::cout << "Color: " << brdf_color.format(ParenthesisFormat) << std::endl;
		//std::cout << "Color: " << objects[select]->color.format(SpaceFormat) << std::endl;
	}
	else
	{
		std::cout << "No Hit" << std::endl;
	}
}

void render(unsigned int width, unsigned int height, Camera &view,
			std::vector<std::shared_ptr<Shape>> &objects,
			std::vector<std::shared_ptr<Light>> &lights, bool use_alt)
{
	const int num_channels = 3;
	const std::string filename = "output.png";
	unsigned char *data = new unsigned char[width * height * num_channels];

	for(unsigned int y = 0; y < height; ++y)
	{
		for(unsigned int x = 0; x < width; ++x)
		{
			double u = -0.5 + ((x + 0.5) / width);
			double v = -0.5 + ((y + 0.5) / height);
			double w = -1;
			double t = std::numeric_limits<double>::max();
			int select = 0;
			bool print = false;
			unsigned char red = 0, green = 0, blue = 0;

			Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
			Eigen::Vector3d look = view.right.cross(view.up.normalized());
			Eigen::Vector3d dis = ((view.right * u) + (view.up.normalized() * v) + (w * look.normalized())).normalized();
			Ray test(view.position, dis);
			Eigen::Vector3d brdf_color;

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

			if(print)
			{
				/*
				red = (unsigned char) std::round(objects[select]->color(0) * 255.f);
				green = (unsigned char) std::round(objects[select]->color(1) * 255.f);
				blue = (unsigned char) std::round(objects[select]->color(2) * 255.f);
				*/
				brdf_color = objects[select]->ambient * objects[select]->color;
				for(unsigned int i = 0; i < lights.size(); i++)
				{
					Eigen::Vector3d v_vec = -dis;
					v_vec.normalize();
					Eigen::Vector3d l_vec = lights[i]->position - test.get_point(t);
					l_vec.normalize();
					Eigen::Vector3d offset = test.get_point(t) + Eigen::Vector3d(0.001, 0.001, 0.001);
					Ray shadow(offset, l_vec);
					Eigen::Vector3d n_vec = objects[select]->get_normal(test.get_point(t));
					n_vec.normalize();
					Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();

					bool is_shadowed = cast_shadow_ray(shadow, objects, *lights[i]);
					if(!is_shadowed)
					{
						Eigen::Vector3d kd = objects[select]->diffuse * objects[select]->color;
						kd(0) *= lights[i]->color(0);
						kd(1) *= lights[i]->color(1);
						kd(2) *= lights[i]->color(2);
						double stuff = clamp(n_vec.dot(l_vec), 0.0, 1.0);
						brdf_color += kd * stuff;
						Eigen::Vector3d ks = objects[select]->specular * objects[select]->color;
						ks(0) *= lights[i]->color(0);
						ks(1) *= lights[i]->color(1);
						ks(2) *= lights[i]->color(2);
						double val = (2 / (std::pow(objects[select]->roughness, 2) - 2));
						double stuff2 = clamp(std::pow(h_vec.dot(n_vec), val), 0.0, 1.0);
						brdf_color += ks * stuff2;
					}
				}
				red = (unsigned char) std::round(brdf_color(0) * 255.f);
				green = (unsigned char) std::round(brdf_color(1) * 255.f);
				blue = (unsigned char) std::round(brdf_color(2) * 255.f);
			}

			data[(width * num_channels) * (height - 1 - y) + num_channels * x + 0] = red;
			data[(width * num_channels) * (height - 1 - y) + num_channels * x + 1] = green;
			data[(width * num_channels) * (height - 1 - y) + num_channels * x + 2] = blue;
		}
	}

	stbi_write_png(filename.c_str(), width, height, num_channels, data, width * num_channels);
	delete[] data;
}

int main(int argc, char *argv[])
{
	std::vector<std::shared_ptr<Shape>> objects;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<unsigned int> options;
	Camera view;
	// bool alternative_brdf = false;

	Command type = is_valid_command(argc, argv);
	parse_scene(argv[2], view, lights, objects);
	parse_optional(argc, argv, options);

	switch(type)
	{
		case Command::RENDER:
			render(options[0], options[1], view, objects, lights, false);
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
		case Command::PIXELCOLOR:
			pixelcolor(options[0], options[1], options[2], options[3], view, objects, lights, false);
			break;
	}
	return 0;
}