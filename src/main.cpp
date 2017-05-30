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

Ray get_pixel_ray(const Scene &scene, uint x, uint y, uint sub_pixel = 0)
{
	double u = -0.5 + ((x + 0.5 + sub_pixel) / scene.width);
	double v = -0.5 + ((y + 0.5 + sub_pixel) / scene.height);
	double w = -1;
	Eigen::Vector3d look = scene.view.right.cross(scene.view.up.normalized());
	Eigen::Vector3d dis = ((scene.view.right * u)
						+ (scene.view.up.normalized() * v)
						+ (w * look.normalized())).normalized();
	Ray pixel_ray(scene.view.position, dis);

	return pixel_ray;
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

std::shared_ptr<Shape> get_shape(const Scene &scene, Ray &ray)
{
	std::shared_ptr<Shape> hit_object = NULL;
	bool collision = false;
	int select = 0;
	double t = std::numeric_limits<double>::max();

	for(uint i = 0; i < scene.shapes.size(); i++)
	{
		double temp = scene.shapes[i]->collision(ray);
		if(temp > 0 && temp < t)
		{
			collision = true;
			select = i;
			t = temp;
		}
	}

	if(collision)
	{
		hit_object = scene.shapes[select];
	}

	return hit_object;
}

double get_intersection_time(std::shared_ptr<Shape> hit_shape, Ray &ray)
{
	return hit_shape ? hit_shape->collision(ray) : std::numeric_limits<double>::max();
}

Eigen::Vector3d get_ambient_color(std::shared_ptr<Shape> hit_shape)
{
	return hit_shape->ambient * hit_shape->color;
}

// TODO(kjayakum): This might be incorrect?
double get_fresnel_contribution(std::shared_ptr<Shape> hit_shape)
{
	double fresnel_0 = std::pow(hit_shape->ior - 1, 2) / std::pow(hit_shape->ior + 1, 2);
	fresnel_0 = hit_shape->reflection > 0 ? fresnel_0 : 0;
	return 0;
}

double get_local_contribution(std::shared_ptr<Shape> hit_shape)
{
	return (1 - hit_shape->filter) * (1 - hit_shape->reflection);
}

double get_reflection_contribution(std::shared_ptr<Shape> hit_shape)
{
	return ((1 - hit_shape->filter) * hit_shape->reflection) + hit_shape->filter * get_fresnel_contribution(hit_shape);
}

double get_transmission_contribution(std::shared_ptr<Shape> hit_shape)
{
	return hit_shape->filter;
}

Eigen::Vector3d get_diffuse_color(const Scene &scene, Ray &ray, std::shared_ptr<Shape> hit_shape)
{
	Eigen::Vector3d diffuse = Eigen::Vector3d::Zero();
	double t = get_intersection_time(hit_shape, ray);
	for(uint i = 0; i < scene.lights.size(); i++)
	{
		Eigen::Vector3d v_vec = -ray.direction;
		v_vec.normalized();
		Eigen::Vector3d l_vec = scene.lights[i].position - ray.get_point(t);
		l_vec.normalize();
		Eigen::Vector3d offset = ray.get_point(t - 0.001);
		Ray shadow(offset, l_vec);
		Eigen::Vector3d n_vec = hit_shape->get_normal(ray.get_point(t));
		n_vec.normalize();
		bool shadowed = cast_shadow_ray(shadow, scene.shapes, scene.lights[i]);
		if(shadowed)
			continue;
		Eigen::Vector3d kd = hit_shape->diffuse * hit_shape->color;
		kd(0) *= scene.lights[i].color(0);
		kd(1) *= scene.lights[i].color(1);
		kd(2) *= scene.lights[i].color(2);
		double stuff = n_vec.dot(l_vec);
		diffuse += kd * stuff;
	}
	diffuse *= get_local_contribution(hit_shape);
	diffuse(0) = clamp(diffuse(0), 0.0, 1.0);
	diffuse(1) = clamp(diffuse(1), 0.0, 1.0);
	diffuse(2) = clamp(diffuse(2), 0.0, 1.0);

	return diffuse;
}

Eigen::Vector3d get_specular_color(const Scene &scene, Ray &ray, std::shared_ptr<Shape> hit_shape)
{
	Eigen::Vector3d specular = Eigen::Vector3d::Zero();
	double t = get_intersection_time(hit_shape, ray);
	for(uint i = 0; i < scene.lights.size(); i++)
	{
		Eigen::Vector3d v_vec = -ray.direction;
		v_vec.normalize();
		Eigen::Vector3d l_vec = scene.lights[i].position - ray.get_point(t);
		l_vec.normalize();
		Eigen::Vector3d offset = ray.get_point(t - 0.001);
		Ray shadow(offset, l_vec);
		Eigen::Vector3d n_vec = hit_shape->get_normal(ray.get_point(t));
		n_vec.normalize();
		Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();
		bool shadowed = cast_shadow_ray(shadow, scene.shapes, scene.lights[i]);
		if(shadowed)
			continue;
		Eigen::Vector3d ks = hit_shape->specular * scene.lights[i].color;
		double power = (2 / (std::pow(hit_shape->roughness, 2)) - 2);
		double stuff2 = clamp(std::pow(n_vec.dot(h_vec), power), 0.0, 1.0);
		specular += ks * stuff2;
	}
	specular *= get_local_contribution(hit_shape);
	specular(0) = clamp(specular(0), 0.0, 1.0);
	specular(1) = clamp(specular(1), 0.0, 1.0);
	specular(2) = clamp(specular(2), 0.0, 1.0);

	return specular;
}

Eigen::Vector3d blinn_phong(const Scene &scene, Ray &ray)
{
	Eigen::Vector3d color = Eigen::Vector3d(0, 0, 0);
	bool collision = false;
	int select = 0;
	double t = std::numeric_limits<double>::max();

	for(uint i = 0; i < scene.shapes.size(); i++)
	{
		double temp = scene.shapes[i]->collision(ray);
		if(temp > 0 && temp < t)
		{
			collision = true;
			select = i;
			t = temp;
		}
	}

	if(collision)
	{
		color += scene.shapes[select]->ambient * scene.shapes[select]->color;
		for(uint i = 0; i < scene.lights.size(); i++)
		{
			Eigen::Vector3d v_vec = -ray.direction;
			v_vec.normalize();
			Eigen::Vector3d l_vec = scene.lights[i].position - ray.get_point(t);
			l_vec.normalize();
			Eigen::Vector3d offset = ray.get_point(t - 0.001);
			Ray shadow(offset, l_vec);
			Eigen::Vector3d n_vec = scene.shapes[select]->get_normal(ray.get_point(t));
			n_vec.normalize();
			Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();
			bool shadowed = cast_shadow_ray(shadow, scene.shapes, scene.lights[i]);
			if(!shadowed)
			{
				Eigen::Vector3d kd = scene.shapes[select]->diffuse * scene.shapes[select]->color;
				kd(0) *= scene.lights[i].color(0);
				kd(1) *= scene.lights[i].color(1);
				kd(2) *= scene.lights[i].color(2);
				double stuff = n_vec.dot(l_vec);
				color += kd * stuff;
				Eigen::Vector3d ks = scene.shapes[select]->specular * scene.lights[i].color;
				double power = (2 / (std::pow(scene.shapes[select]->roughness, 2)) - 2);
				double stuff2 = clamp(std::pow(n_vec.dot(h_vec), power), 0.0, 1.0);
				color += ks * stuff2;
			}
		}
	}

	color(0) = clamp(color(0), 0.0, 1.0);
	color(1) = clamp(color(1), 0.0, 1.0);
	color(2) = clamp(color(2), 0.0, 1.0);

	return color;
}

// TODO(kjayakum): Setup alternative shader besides blinn_phong
Eigen::Vector3d get_reflection(const Scene &scene, Ray &ray, int depth = 0)
{
	Eigen::Vector3d color = Eigen::Vector3d(0, 0, 0);
	bool collision = false;
	int select = 0;
	double t = std::numeric_limits<double>::max();

	for(uint i = 0; i < scene.shapes.size(); i++)
	{
		double temp = scene.shapes[i]->collision(ray);
		if(temp > 0 && temp < t)
		{
			collision = true;
			select = i;
			t = temp;
		}
	}

	if(collision)
	{
		color = 0.5 * blinn_phong(scene, ray);
		if(scene.shapes[select]->reflection > 0 && depth <= 6)
		{
			Eigen::Vector3d n_vec = scene.shapes[select]->get_normal(ray.get_point(t));
			n_vec.normalize();
			Eigen::Vector3d r_vec = ray.direction - (2 * (ray.direction.dot(n_vec)) * n_vec);
			r_vec.normalize();
			Ray reflection = Ray(ray.get_point(t - 0.001), r_vec);
			color += scene.shapes[select]->reflection * get_reflection(scene, reflection, depth + 1);
		}
	}

	color(0) = clamp(color(0), 0.0, 1.0);
	color(1) = clamp(color(1), 0.0, 1.0);
	color(2) = clamp(color(2), 0.0, 1.0);
	return color;
}

Eigen::Vector3d get_refraction(const Scene &scene, Ray &ray, int depth = 0, double ior = 1.0)
{
	Eigen::Vector3d color = Eigen::Vector3d(0, 0, 0);
	bool collision = false;
	int select = 0;
	double t = std::numeric_limits<double>::max();

	for(uint i = 0; i < scene.shapes.size(); i++)
	{
		double temp = scene.shapes[i]->collision(ray);
		if(temp > 0 && temp < t)
		{
			collision = true;
			select = i;
			t = temp;
		}
	}

	if(collision)
	{
		color = 0.5 * blinn_phong(scene, ray);
		if((scene.shapes[select]->refraction > 0 || scene.shapes[select]->filter> 0) && depth <= 6)
		{
			double final_ior;
			double ior2 = scene.shapes[select]->ior;
			Eigen::Vector3d n = scene.shapes[select]->get_normal(ray.get_point(t));
			Eigen::Vector3d d = ray.direction;
			// Check if ray is outside an object
			if(d.dot(n) < 0)
			{
				final_ior = ior / ior2;
			}
			else
			{
				n = -n;
				final_ior = ior2 / ior;
			}
			double discriminant = 1 - (std::pow((final_ior), 2) * (1 - std::pow(d.dot(n), 2)));
			Eigen::Vector3d t_vec = (final_ior) * (d - (d.dot(n) * n));
			t_vec = t_vec - (n * std::sqrt(discriminant));
			t_vec.normalize();
			Ray refract(ray.get_point(t + 0.001), t_vec);
			if(scene.shapes[select]->refraction > 0)
				color += scene.shapes[select]->refraction * get_refraction(scene, refract, depth + 1);
			else
				color += scene.shapes[select]->filter * get_refraction(scene, refract, depth + 1);
		}
	}

	color(0) = clamp(color(0), 0.0, 1.0);
	color(1) = clamp(color(1), 0.0, 1.0);
	color(2) = clamp(color(2), 0.0, 1.0);
	return color;
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

Eigen::Vector3d get_pixel_color(const Scene &scene, uint x, uint y, uint sample_size = 1)
{
	Eigen::Vector3d pixel_color = Eigen::Vector3d::Zero();
	for(uint num_sample = 0; num_sample < sample_size; num_sample++)
	{
		double sub_pixel = -0.5 + ((num_sample + 0.5) / sample_size);
		Ray pixel_ray = get_pixel_ray(scene, x, y, sub_pixel);
		std::shared_ptr<Shape> hit_shape = get_shape(scene, pixel_ray);
		Eigen::Vector3d local_color = blinn_phong(scene, pixel_ray);
		Eigen::Vector3d reflection_color = get_reflection(scene, pixel_ray);
		Eigen::Vector3d refraction_color = get_refraction(scene, pixel_ray);
		Eigen::Vector3d sample_color;

		if(hit_shape)
		{
			double local_contribution = get_local_contribution(hit_shape);
			double reflection_contribution = get_reflection_contribution(hit_shape);
			double refraction_contribution = hit_shape->filter;
			sample_color = local_contribution * local_color
						+ reflection_contribution * reflection_color
						+ refraction_contribution * refraction_color;
		}
		else
			sample_color = local_color + reflection_color + refraction_color;
		sample_color(0) = clamp(sample_color(0), 0.0, 1.0);
		sample_color(1) = clamp(sample_color(1), 0.0, 1.0);
		sample_color(2) = clamp(sample_color(2), 0.0, 1.0);
		pixel_color += sample_color;
	}
	pixel_color /= sample_size;
	return pixel_color;
}

uint get_shape_id(const Scene &scene, std::shared_ptr<Shape> search)
{
	uint shape_id;

	for(uint i = 0; i < scene.shapes.size(); i++)
	{
		if(search == scene.shapes[i])
		{
			shape_id = i + 1;
			break;
		}
	}

	return shape_id;
}

Ray get_object_ray(const Ray &pixel_ray, std::shared_ptr<Shape> object)
{
	Eigen::Vector4d transform_dir;
	Eigen::Vector4d transform_pos;
	transform_dir << pixel_ray.direction.head<3>(), 0;
	transform_pos << pixel_ray.origin.head<3>(), 1;
	transform_dir = object->inverse_transform * transform_dir;
	transform_pos = object->inverse_transform * transform_pos;
	Eigen::Vector3d direction = transform_dir.head<3>();
	Eigen::Vector3d position = transform_pos.head<3>();
	return Ray(position, direction);
}

// TODO(kjayakum): Include refraction/reflection prints
void printrays(const Scene &scene, uint x, uint y, uint depth = 0, std::string name = "Primary")
{
	Ray pixel_ray = get_pixel_ray(scene, x, y);
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
	std::shared_ptr<Shape> hit_shape = get_shape(scene, pixel_ray);
	double intersection_time = get_intersection_time(hit_shape, pixel_ray);

	std::cout << "----" << std::endl;
	std::cout << std::setw(18) << "Iteration type: " << name << std::endl;
	std::cout << std::setw(18) << "Ray: " << "{" << pixel_ray.origin.format(SpaceFormat);
	std::cout << "} -> {" << pixel_ray.direction.format(SpaceFormat) << "}" << std::endl;

	if(hit_shape)
	{

		if(hit_shape->inverse_transform != Eigen::Matrix4d::Identity())
		{
			std::cout << std::setw(18) << "Transformed Ray: ";
			Ray object_ray = get_object_ray(pixel_ray, hit_shape);
			std::cout << "{" << object_ray.origin.format(SpaceFormat);
			std::cout << "} -> {" << object_ray.direction.format(SpaceFormat) << "}" << std::endl; 
		}
		std::cout << std::setw(18) << "Hit Object: " << "(ID #" << get_shape_id(scene, hit_shape);
		std::cout << " - ";
		hit_shape->print_type(std::cout);
		std::cout << ")" << std::endl;

		std::cout << std::setw(18) << "Intersection: ";
		std::cout << "{" << pixel_ray.get_point(intersection_time).format(SpaceFormat) << "} ";
		std::cout << "at T = " << intersection_time << std::endl;

		std::cout << std::setw(18) << "Normal: ";
		std::cout << "{" << hit_shape->get_normal(pixel_ray.get_point(intersection_time)).format(SpaceFormat);
		std::cout << "}" << std::endl;
		
		std::cout << std::setw(18) << "Final Color: ";
		std::cout << "{" << get_pixel_color(scene, x , y).format(SpaceFormat) << "}" << std::endl;

		std::cout << std::setw(18) << "Ambient: ";
		std::cout << "{" << get_ambient_color(hit_shape).format(SpaceFormat) << "}" << std::endl;

		std::cout << std::setw(18) << "Diffuse: ";
		std::cout << "{" << get_diffuse_color(scene, pixel_ray, hit_shape).format(SpaceFormat) << "}";
		std::cout << std::endl;

		std::cout << std::setw(18) << "Specular: ";
		std::cout << "{" << get_specular_color(scene, pixel_ray, hit_shape).format(SpaceFormat) << "}";
		std::cout << std::endl;

		std::cout << std::setw(18) << "Reflection: ";
		Eigen::Vector3d reflection = get_reflection(scene, pixel_ray);
		reflection *= get_reflection_contribution(hit_shape);
		std::cout << "{" << reflection.format(SpaceFormat) << "}" << std::endl;

		std::cout << std::setw(18) << "Refraction: ";
		Eigen::Vector3d refraction = get_refraction(scene, pixel_ray);
		refraction *= get_transmission_contribution(hit_shape);
		std::cout << "{" << refraction.format(SpaceFormat) << "}" << std::endl;

		std::cout << std::setw(18) << "Contributions: ";
		std::cout << std::fixed << std::setprecision(4) << get_local_contribution(hit_shape);
		std::cout << " Local, ";
		std::cout << std::fixed << std::setprecision(4) << get_reflection_contribution(hit_shape);
		std::cout << " Reflection, ";
		std::cout << std::fixed << std::setprecision(4) << get_transmission_contribution(hit_shape);
		std::cout << " Transmission";
		std::cout << std::endl;
	}
}

void render(const Scene &scene, uint sample_size, bool use_alt = false)
{
	const int num_channels = 3;
	const std::string filename = "output.png";
	unsigned char *data = new unsigned char[scene.width * scene.height * num_channels];

	for(unsigned int y = 0; y < scene.height; ++y)
	{
		for(unsigned int x = 0; x < scene.width; ++x)
		{
			unsigned char red = 0, green = 0, blue = 0;
			Eigen::Vector3d pixel_color = get_pixel_color(scene, x, y, sample_size);

			red = (unsigned char) std::round(pixel_color(0) * 255.f);
			green = (unsigned char) std::round(pixel_color(1) * 255.f);
			blue = (unsigned char) std::round(pixel_color(2) * 255.f);
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
			render(scene, get_supersample(argc, argv));
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
		case Command::PRINTRAYS:
			scene.set_scene_dimensions(options[0], options[1]);
			std::cout << "Pixel: [" << options[2] << ", " << options[3] << "] ";
			// TODO(kjayakum): Add Pixel color final print out
			std::cout << "Color: (" << std::endl;
			printrays(scene, options[2], options[3]);
			break;
	}
	return 0;
}