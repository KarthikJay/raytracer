#ifndef UTILITY_HPP
#define UTILITY_HPP

enum class Command : int 
{
	RENDER,
	SCENEINFO,
	PIXELRAY,
	FIRSTHIT,
	PIXELCOLOR,
	PIXELTRACE,
	PRINTRAYS
};

enum class Flags : int
{
	ALTERNATIVE_BRDF,
	FRESNEL,
	SPACIAL_DATA_STRUCTURES
};

void parse_optional(int argc, char *argv[], std::vector<unsigned int> &optional);
Command is_valid_command(int argc, char *argv[], std::vector<uint> &options);
uint get_supersample(int argc, char *argv[]);
void get_flags(int argc, char *argv[], std::array<bool, 3> &flags);

#endif