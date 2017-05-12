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
#include "scene.hpp"
#include "shader.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template <typename T>
T clamp(const T n, const T lower, const T upper)
{
	return std::max(lower, std::min(n, upper));
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
	std::cout << "Ray: {" << test.position.format(SpaceFormat) << "} -> {";
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
		objects[select]->print_type(std::cout);
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

	temp.position = view.position;
	std::cout << "Pixel: [" << x << ", " << y << "] ";
	std::cout << "Ray: {" << temp.position.format(SpaceFormat) << "} -> {";

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
				double s = (hit_point - test.position).norm();
				double check = (cur_light.position - test.position).norm();
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
	std::cout << "Ray: {" << test.position.format(SpaceFormat) << "} -> {";
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
		objects[select]->print_type(std::cout);
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
			Eigen::Vector3d offset = test.get_point(t - 0.001);
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
				Eigen::Vector3d ks = objects[select]->specular * lights[i]->color;
				double val = (2 / (std::pow(objects[select]->roughness, 2)) - 2);
				double stuff2 = clamp(std::pow(n_vec.dot(h_vec), val), 0.0, 1.0);
				brdf_color += ks * stuff2;
				brdf_color(0) = clamp(brdf_color(0), 0.0, 1.0);
				brdf_color(1) = clamp(brdf_color(1), 0.0, 1.0);
				brdf_color(2) = clamp(brdf_color(2), 0.0, 1.0);
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
					Eigen::Vector3d offset = test.get_point(t - 0.001);
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
						Eigen::Vector3d ks = objects[select]->specular * lights[i]->color;
						double val = (2 / (std::pow(objects[select]->roughness, 2)) - 2);
						double stuff2 = clamp(std::pow(n_vec.dot(h_vec), val), 0.0, 1.0);
						brdf_color += ks * stuff2;
						brdf_color(0) = clamp(brdf_color(0), 0.0, 1.0);
						brdf_color(1) = clamp(brdf_color(1), 0.0, 1.0);
						brdf_color(2) = clamp(brdf_color(2), 0.0, 1.0);
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
	Camera view;
	std::ifstream input_file;
	std::vector<unsigned int> options;
	// bool alternative_brdf = false;
	Command type = is_valid_command(argc, argv);
	parse_optional(argc, argv, options);
	Scene scene(options[0], options[1]);
	input_file.open(argv[2]);
	input_file >> scene;
	input_file.close();
	Shader test(scene);
	//scene.print_visual(std::cout);

	switch(type)
	{
		case Command::RENDER:
			//render(options[0], options[1], view, objects, lights, false);
			break;
		case Command::FIRSTHIT:
			//firsthit(options[0], options[1], options[2], options[3], view, objects);
			test.set_pixel(options[2], options[3]);
			test.print_collision(std::cout);
			break;
		case Command::PIXELRAY:
			//pixelray(options[0], options[1], options[2], options[3], view);
			test.set_pixel(options[2], options[3]);
			test.print_pixel_ray(std::cout);
			break;
		case Command::SCENEINFO:
			//print_scene(view, objects, lights);
			std::cout << scene;
			break;
		case Command::PIXELCOLOR:
			//pixelcolor(options[0], options[1], options[2], options[3], view, objects, lights, false);
			test.set_pixel(options[2], options[3]);
			test.print_pixel_color(std::cout);
			break;
	}

	return 0;
}