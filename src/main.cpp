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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template <typename T>
T clamp(const T n, const T lower, const T upper)
{
	return std::max(lower, std::min(n, upper));
}

void firsthit(const Scene &scene, uint x, uint y)
{
	double u = -0.5 + ((x + 0.5) / scene.width);
	double v = -0.5 + ((y + 0.5) / scene.height);
	double w = -1;
	double t = std::numeric_limits<double>::max();
	int select = 0;
	bool print = false;
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	Eigen::Vector3d look = scene.view.right.cross(scene.view.up.normalized());
	Eigen::Vector3d dis = ((scene.view.right * u)
						+ (scene.view.up.normalized() * v)
						+ (w * look.normalized())).normalized();
	Ray test(scene.view.position, dis);

	std::cout << "Pixel: [" << x << ", " << y << "] ";
	std::cout << "Ray: {" << test.origin.format(SpaceFormat) << "} -> {";
	std::cout << test.direction.format(SpaceFormat) << "}" << std::endl;

	// Loop through objects checking collision
	for(unsigned int i = 0; i < scene.shapes.size(); i++)
	{
		double temp = scene.shapes[i]->collision(test);
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
		scene.shapes[select]->print_type(std::cout);
		std::cout << std::endl;
		std::cout << "Color: " << scene.shapes[select]->color.format(SpaceFormat) << std::endl;
	}
	else
	{
		std::cout << "No Hit" << std::endl;
	}
}

void pixelray(const Scene &scene, uint x, uint y)
{
	Ray temp;
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	Eigen::Vector3d look = scene.view.right.cross(scene.view.up.normalized());
	Eigen::Vector3d dist;

	temp.origin = scene.view.position;
	std::cout << "Pixel: [" << x << ", " << y << "] ";
	std::cout << "Ray: {" << temp.origin.format(SpaceFormat) << "} -> {";

	double u = -0.5 + ((x + 0.5) / scene.width);
	double v = -0.5 + ((y + 0.5) / scene.height);
	double w = -1;

	dist = (scene.view.right * u) + (scene.view.up.normalized() * v) + (w * look.normalized());
	temp.direction = dist.normalized();
	std::cout << temp.direction.format(SpaceFormat) << "}" << std::endl;
}

bool cast_shadow_ray(Ray &test, const std::vector<std::shared_ptr<Shape>> &objects,
					 const Light &cur_light)
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

Eigen::Vector3d cast_reflect_ray(const Scene &scene, Ray &ray, uint depth = 0)
{
	Eigen::Vector3d hit_color = Eigen::Vector3d(0, 0, 0);
	int select = 0;
	bool print = false;
	double t = std::numeric_limits<double>::max();
	if(depth != 6)
	{
		for(unsigned int i = 0; i < scene.shapes.size(); i++)
		{
			double temp = scene.shapes[i]->collision(ray);
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
			for(uint i = 0; i < scene.lights.size(); i++)
			{
				Eigen::Vector3d n_vec = scene.shapes[select]->get_normal(ray.get_point(t));
				n_vec.normalize();
				Eigen::Vector3d r_vec = ray.direction - (2 * (ray.direction.dot(n_vec)) * n_vec);
				r_vec.normalize();
				Ray reflection = Ray(ray.get_point(t - 0.001), r_vec);
				// TODO(kjayakum): Refactor to get an intersected objects blinn_phong color
				hit_color += scene.shapes[select]->color * scene.shapes[select]->ambient;
				hit_color += cast_reflect_ray(scene, reflection, depth + 1);
			}
		}
	}
	return hit_color;
}

void pixelcolor(const Scene &scene, uint x, uint y, bool use_alt = false)
{
	Eigen::Vector3d brdf_color;
	double u = -0.5 + ((x + 0.5) / scene.width);
	double v = -0.5 + ((y + 0.5) / scene.height);
	double w = -1;
	double t = std::numeric_limits<double>::max();
	int select = 0;
	bool print = false;
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	Eigen::IOFormat ParenthesisFormat(4, Eigen::DontAlignCols, "", ", ", "", "", "(", ")");
	Eigen::Vector3d look = scene.view.right.cross(scene.view.up.normalized());
	Eigen::Vector3d dis = ((scene.view.right * u) + (scene.view.up.normalized() * v) + (w * look.normalized())).normalized();
	Ray test(scene.view.position, dis);

	std::cout << "Pixel: [" << x << ", " << y << "] ";
	std::cout << "Ray: {" << test.origin.format(SpaceFormat) << "} -> {";
	std::cout << test.direction.format(SpaceFormat) << "}" << std::endl;

	// Loop through objects checking collision
	for(unsigned int i = 0; i < scene.shapes.size(); i++)
	{
		double temp = scene.shapes[i]->collision(test);
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
		scene.shapes[select]->print_type(std::cout);
		std::cout << std::endl;
		std::cout << "BRDF: ";
		if(!use_alt)
			std::cout << "Blinn-Phong";
		else
			std::cout << "Alternate";
		std::cout << std::endl;
		brdf_color = scene.shapes[select]->ambient * scene.shapes[select]->color;
		for(unsigned int i = 0; i < scene.lights.size(); i++)
		{
			Eigen::Vector3d v_vec = -dis;
			v_vec.normalize();
			Eigen::Vector3d l_vec = scene.lights[i].position - test.get_point(t);
			l_vec.normalize();
			Eigen::Vector3d offset = test.get_point(t - 0.001);
			Ray shadow(offset, l_vec);
			Eigen::Vector3d n_vec = scene.shapes[select]->get_normal(test.get_point(t));
			n_vec.normalize();
			Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();
			//Eigen::Vector3d r_vec = l_vec - (2 * (l_vec.dot(n_vec)) * n_vec);
			//r_vec.normalize();
			bool is_shadowed = cast_shadow_ray(shadow, scene.shapes, scene.lights[i]);
			if(!is_shadowed)
			{
				Eigen::Vector3d kd = scene.shapes[select]->diffuse * scene.shapes[select]->color;
				kd(0) *= scene.lights[i].color(0);
				kd(1) *= scene.lights[i].color(1);
				kd(2) *= scene.lights[i].color(2);
				double stuff = n_vec.dot(l_vec);
				brdf_color += kd * stuff;
				Eigen::Vector3d ks = scene.shapes[select]->specular * scene.lights[i].color;
				double val = (2 / (std::pow(scene.shapes[select]->roughness, 2)) - 2);
				double stuff2 = clamp(std::pow(n_vec.dot(h_vec), val), 0.0, 1.0);
				brdf_color += ks * stuff2;
				brdf_color(0) = clamp(brdf_color(0), 0.0, 1.0);
				brdf_color(1) = clamp(brdf_color(1), 0.0, 1.0);
				brdf_color(2) = clamp(brdf_color(2), 0.0, 1.0);
			}
		}
		brdf_color += scene.shapes[select]->reflection * cast_reflect_ray(scene, test);
		brdf_color(0) = clamp(brdf_color(0), 0.0, 1.0);
		brdf_color(1) = clamp(brdf_color(1), 0.0, 1.0);
		brdf_color(2) = clamp(brdf_color(2), 0.0, 1.0);
		brdf_color *= 255;
		brdf_color(0) = std::round(brdf_color(0));
		brdf_color(1) = std::round(brdf_color(1));
		brdf_color(2) = std::round(brdf_color(2));
		std::cout << "Color: " << brdf_color.format(ParenthesisFormat) << std::endl;
	}
	else
	{
		std::cout << "No Hit" << std::endl;
	}
}

void render(const Scene &scene, bool use_alt = false)
{
	const int num_channels = 3;
	const std::string filename = "output.png";
	unsigned char *data = new unsigned char[scene.width * scene.height * num_channels];

	for(unsigned int y = 0; y < scene.height; ++y)
	{
		for(unsigned int x = 0; x < scene.width; ++x)
		{
			double u = -0.5 + ((x + 0.5) / scene.width);
			double v = -0.5 + ((y + 0.5) / scene.height);
			double w = -1;
			double t = std::numeric_limits<double>::max();
			int select = 0;
			bool print = false;
			unsigned char red = 0, green = 0, blue = 0;

			Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
			Eigen::Vector3d look = scene.view.right.cross(scene.view.up.normalized());
			Eigen::Vector3d dis = ((scene.view.right * u) + (scene.view.up.normalized() * v) + (w * look.normalized())).normalized();
			Ray test(scene.view.position, dis);
			Eigen::Vector3d brdf_color;

			// Loop through objects checking collision
			for(unsigned int i = 0; i < scene.shapes.size(); i++)
			{
				double temp = scene.shapes[i]->collision(test);
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
				brdf_color = scene.shapes[select]->ambient * scene.shapes[select]->color;
				for(unsigned int i = 0; i < scene.lights.size(); i++)
				{
					Eigen::Vector3d v_vec = -dis;
					v_vec.normalize();
					Eigen::Vector3d l_vec = scene.lights[i].position - test.get_point(t);
					l_vec.normalize();
					Eigen::Vector3d offset = test.get_point(t - 0.001);
					Ray shadow(offset, l_vec);
					Eigen::Vector3d n_vec = scene.shapes[select]->get_normal(test.get_point(t));
					n_vec.normalize();
					Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();

					bool is_shadowed = cast_shadow_ray(shadow, scene.shapes, scene.lights[i]);
					if(!is_shadowed)
					{
						Eigen::Vector3d kd = scene.shapes[select]->diffuse * scene.shapes[select]->color;
						kd(0) *= scene.lights[i].color(0);
						kd(1) *= scene.lights[i].color(1);
						kd(2) *= scene.lights[i].color(2);
						double stuff = clamp(n_vec.dot(l_vec), 0.0, 1.0);
						brdf_color += kd * stuff;
						Eigen::Vector3d ks = scene.shapes[select]->specular * scene.lights[i].color;
						double val = (2 / (std::pow(scene.shapes[select]->roughness, 2)) - 2);
						double stuff2 = clamp(std::pow(n_vec.dot(h_vec), val), 0.0, 1.0);
						brdf_color += ks * stuff2;
						brdf_color(0) = clamp(brdf_color(0), 0.0, 1.0);
						brdf_color(1) = clamp(brdf_color(1), 0.0, 1.0);
						brdf_color(2) = clamp(brdf_color(2), 0.0, 1.0);
					}
				}
				brdf_color += scene.shapes[select]->reflection * cast_reflect_ray(scene, test);
				brdf_color(0) = clamp(brdf_color(0), 0.0, 1.0);
				brdf_color(1) = clamp(brdf_color(1), 0.0, 1.0);
				brdf_color(2) = clamp(brdf_color(2), 0.0, 1.0);
				red = (unsigned char) std::round(brdf_color(0) * 255.f);
				green = (unsigned char) std::round(brdf_color(1) * 255.f);
				blue = (unsigned char) std::round(brdf_color(2) * 255.f);
			}
			data[(scene.width * num_channels) * (scene.height - 1 - y) + num_channels * x + 0] = red;
			data[(scene.width * num_channels) * (scene.height - 1 - y) + num_channels * x + 1] = green;
			data[(scene.width * num_channels) * (scene.height - 1 - y) + num_channels * x + 2] = blue;
		}
	}

	stbi_write_png(filename.c_str(), scene.width, scene.height, num_channels, data, scene.width * num_channels);
	delete[] data;
}

int main(int argc, char *argv[])
{
	Scene scene;
	Camera view;
	std::ifstream input_file;
	std::vector<unsigned int> options;

	input_file.open(argv[2]);
	input_file >> scene;
	input_file.close();
	Command type = is_valid_command(argc, argv);
	parse_optional(argc, argv, options);

	switch(type)
	{
		case Command::RENDER:
			scene.set_scene_dimensions(options[0], options[1]);
			render(scene);
			break;
		case Command::FIRSTHIT:
			scene.set_scene_dimensions(options[0], options[1]);
			firsthit(scene, options[2], options[3]);
			break;
		case Command::PIXELRAY:
			scene.set_scene_dimensions(options[0], options[1]);
			pixelray(scene, options[2], options[3]);
			break;
		case Command::SCENEINFO:
			std::cout << scene;
			break;
		case Command::PIXELCOLOR:
			scene.set_scene_dimensions(options[0], options[1]);
			pixelcolor(scene, options[2], options[3]);
			break;
		case Command::PIXELTRACE:
			scene.set_scene_dimensions(options[0], options[1]);
			break;
	}
	return 0;
}