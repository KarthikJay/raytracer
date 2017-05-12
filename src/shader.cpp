#include <algorithm>
#include <iomanip>

#include "shader.hpp"

Eigen::IOFormat SpaceFormat(4, Eigen::DontAlignCols, " ", " ", "", "", "", "");
Eigen::IOFormat ParenthesisFormat(4, Eigen::DontAlignCols, "", ", ", "", "", "(", ")");

template <typename T>
T clamp(const T n, const T lower, const T upper)
{
	return std::max(lower, std::min(n, upper));
}

// returns double max value if no collision found
double Shader::get_collision_time(const Ray &ray, double time_limit) const
{
	double collision_time = time_limit;
	std::shared_ptr<Shape> collided_object = NULL;

	for(std::shared_ptr<Shape> itr : scene->shapes)
	{
		double hit_time = itr->collision(ray);
		collision_time = (hit_time > 0 && hit_time < collision_time) ? hit_time : collision_time;
	}

	return collision_time;
}

// returns NULL if no collision found at time t along ray.
std::shared_ptr<Shape> Shader::get_collision_shape(const Ray &ray, double time, double delta) const
{
	std::shared_ptr<Shape> collided_object = NULL;

	for(std::shared_ptr<Shape> itr : scene->shapes)
	{
		double hit_time = itr->collision(ray);
		collided_object = (hit_time >= time - delta && hit_time <= time + delta) ? itr : collided_object;
	}

	return collided_object;
}

bool Shader::is_shadowed(const Ray &shadow, const Light &light) const
{
	bool is_shadowed = false;
	double hit_time = get_collision_time(shadow);
	std::shared_ptr<Shape> hit_shape = get_collision_shape(shadow, hit_time);

	if(hit_shape != NULL)
	{
		Eigen::Vector3d hit_point = shadow.get_point(hit_time);
		double d = (hit_point - shadow.position).norm();
		double check = (light.position - shadow.position).norm();
		is_shadowed = d < check ? true : false;
	}

	return is_shadowed;
}

// TODO(kjayakum): Use Eigen::Arrays for points & color, and Eigen::Vectors for actual vectors
// TODO(kjayakum): Fix the blinn_phong
Eigen::Array3d Shader::blinn_phong() const
{
	Eigen::Array3d ka, kd, ks, color;
	Eigen::Vector3d v_vec = -pixel_ray.direction;
	double hit_time = get_collision_time(pixel_ray);
	std::shared_ptr<Shape> hit_shape = get_collision_shape(pixel_ray, hit_time);
	Eigen::Vector3d hit_point = pixel_ray.get_point(hit_time);

	ka = hit_shape->ambient * hit_shape->color;
	color += ka;
	for(Light light : scene->lights)
	{
		Eigen::Vector3d l_vec = light.position - hit_point;
		l_vec.normalize();
		Eigen::Vector3d offset_pos = pixel_ray.get_point(hit_time - epsilon);
		Ray shadow = Ray(offset_pos, l_vec);
		Eigen::Vector3d n_vec = hit_shape->get_normal(hit_point);
		n_vec.normalize();
		Eigen::Vector3d h_vec = (v_vec + l_vec).normalized();
		if(!is_shadowed(shadow, light))
		{
			kd = hit_shape->diffuse * hit_shape->color;
			Eigen::Array3d ld;
			ld(0) = light.color(0);
			ld(1) = light.color(1);
			ld(2) = light.color(2);
			kd = kd * ld * n_vec.dot(l_vec);
			color += kd * stuff;
			ks = hit_shape->specular * light.color;
			double power = (2 / (std::pow(hit_shape->roughness, 2)) - 2);
			ks = ks * clamp(std::pow(n_vec.dot(h_vec), power), 0.0, 1.0);
			color += ks;
			color(0) = clamp(color(0), 0.0, 1.0);
			color(1) = clamp(color(1), 0.0, 1.0);
			color(2) = clamp(color(2), 0.0, 1.0);
		}
	}
	return color;
}

// TODO(kjayakum): Might need to refactor this as more members get added
void Shader::update_member_variables()
{
	// U coordinate
	view_space_point(0) = -0.5 + ((pixel_x + 0.5) / scene->width);
	// V coordinate
	view_space_point(1) = -0.5 + ((pixel_y + 0.5) / scene->height);
	// W coordinate
	view_space_point(2) = -1;

	Eigen::Vector3d look_dir = scene->view.right.cross(scene->view.up.normalized());
	Eigen::Vector3d pixel_ray_dir = (scene->view.right * view_space_point(0))
									+ (scene->view.up.normalized() * view_space_point(1))
									+ (look_dir.normalized() * view_space_point(2));
	pixel_ray_dir.normalize();

	pixel_ray.position = scene->view.position;
	pixel_ray.direction = pixel_ray_dir;
	look_vector = -pixel_ray.direction;

	// TODO(kjayakum): Add updating the color
	// if(shading == BRDF::BLINN_PHONG)

}

void Shader::set_pixel(uint pixel_x, uint pixel_y)
{
	this->pixel_x = pixel_x;
	this->pixel_y = pixel_y;
	update_member_variables();
}

void Shader::set_scene(Scene &scene)
{
	this->scene = &scene;
	update_member_variables();
}

void Shader::print_pixel_ray(std::ostream &out) const
{
	out << "Pixel: [" << pixel_x << ", " << pixel_y << "] ";
	out << "Ray: {" << pixel_ray.position.format(SpaceFormat) << "} -> {";
	out << pixel_ray.direction.format(SpaceFormat) << "}" << std::endl;
}

void Shader::print_collision(std::ostream &out) const
{
	double time = get_collision_time(pixel_ray);
	std::shared_ptr<Shape> shape = get_collision_shape(pixel_ray, time);
	print_pixel_ray(out);

	if(shape != NULL)
	{
		out << "T = " << std::setprecision(4) << time << std::endl;
		out << "Object Type: ";
		shape->print_type(out);
		out << std::endl;
		out << "Color: " << shape->color.format(SpaceFormat) << std::endl;
	}
	else
		std::cout << "No Hit" << std::endl;
}

void Shader::print_pixel_color(std::ostream &out) const
{
	Eigen::Array3d brdf_color = blinn_phong(); 
	double time = get_collision_time(pixel_ray);
	std::shared_ptr<Shape> shape = get_collision_shape(pixel_ray, time);
	print_collision(out);

	brdf_color *= 255;
	brdf_color(0) = std::round(brdf_color(0));
	brdf_color(1) = std::round(brdf_color(1));
	brdf_color(2) = std::round(brdf_color(2));

	if(shape != NULL)
	{
		out << "BRDF: ";
		if(shading == BRDF::BLINN_PHONG)
			out << "Blinn-Phong";
		else if(shading == BRDF::COOK_TORRANCE)
			out << "Cook-Torrance";
		out << std::endl;
		out << "Color: " << brdf_color.format(ParenthesisFormat) << std::endl;
	}
}

Shader::Shader(Scene &scene)
{
	this->scene = &scene;
	shading = BRDF::BLINN_PHONG;
	set_pixel(0, 0);
}