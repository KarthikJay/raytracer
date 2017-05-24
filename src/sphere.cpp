#include <math.h>
#include <algorithm>

#include "sphere.hpp"

Sphere::Sphere()
{
	center = Eigen::Vector3d(0.0, 0.0, 0.0);
	radius = 0.0;

	color = Eigen::Vector3d(0.0, 0.0, 0.0);
	inverse_transform = Eigen::Matrix4d::Identity();
	ambient = 0.0;
	diffuse = 0.0;
	specular = 0.0;
	roughness = 0.5;
	reflection = 0.0;
	refraction = 0.0;
	metallic = 0.0;
	ior = 1.0;
	filter = 0.0;
}

Sphere::Sphere(Eigen::Vector3d center, double radius,
			Eigen::Vector3d color, double ambient, double diffuse,
			double specular, double roughness)
{
	this->center = center;
	this->radius = radius;

	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->roughness = roughness;
}

void Sphere::print(std::ostream &out) const
{
	Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");

	out << "- Type: Sphere" << std::endl;
	out << "- Center: {" << this->center.format(SpaceFormat) << "}" << std::endl;
	out << "- Radius: " << this->radius << std::endl;
	out << "- Color: {" << this->color.format(SpaceFormat) << "}" << std::endl;
	out << "- Material:" << std::endl;
	out << "  - Ambient: " << this->ambient << std::endl;
	out << "  - Diffuse: " << this->diffuse << std::endl;
	out << "  - Specular: " << this->specular << std::endl;
	out << "  - Roughness: " << this->roughness << std::endl;
	out << "  - Reflection: " << this->reflection << std::endl;
	out << "  - Refraction: " << this->refraction << std::endl;
	out << "  - Metallic: " << this->metallic << std::endl;
	out << "  - Index of Refraction: " << this->ior << std::endl;
	out << "  - Filter: " << this->filter << std::endl;
}

// TODO(kjayakum): Fix the transform stuff
double Sphere::collision(Ray &r)
{
	Eigen::Vector4d transform_ray = Eigen::Vector4d::Zero();
	Eigen::Vector4d transform_pos = Eigen::Vector4d::Zero();
	transform_ray(0) = r.direction(0);
	transform_ray(1) = r.direction(1);
	transform_ray(2) = r.direction(2);
	transform_pos(0) = r.origin(0);
	transform_pos(1) = r.origin(1);
	transform_pos(2) = r.origin(2);
	transform_pos(3) = 1;
	transform_ray = this->inverse_transform * transform_ray;
	transform_pos = this->inverse_transform * transform_pos;
	Ray object_ray(Eigen::Vector3d(transform_pos(0), transform_pos(1), transform_pos(2)),
					Eigen::Vector3d(transform_ray(0), transform_ray(1), transform_ray(2)));
	double t1, t2;
	double T = 0.0;
	double A = object_ray.direction.dot(object_ray.direction);
	double B = (2 * object_ray.direction).dot(object_ray.origin - this->center);
	double C = (object_ray.origin - this->center).dot(object_ray.origin - this->center) - (this->radius * this->radius);

	// Positive
	t1 = (-B + std::sqrt(std::pow(B, 2) - (4 * A * C))) / (2 * A);
	// Negative
	t2 = (-B - std::sqrt(std::pow(B, 2) - (4 * A * C))) / (2 * A);

	T = std::min(t1, t2);
	// Remove negative time
	T = std::max(0.0, T);
	return T;
}

void Sphere::print_type(std::ostream &out) const
{
	out << "Sphere";
}

Eigen::Vector3d Sphere::get_normal(Eigen::Vector3d point)
{
	Eigen::Vector3d normal = (point - center).normalized();
	Eigen::Vector4d temp = Eigen::Vector4d::Zero();
	temp(0) = normal(0);
	temp(1) = normal(1);
	temp(2) = normal(2);
	temp = this->inverse_transform.transpose() * temp;
	normal(0) = temp(0);
	normal(1) = temp(1);
	normal(2) = temp(2);
	return normal;
}