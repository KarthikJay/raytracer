#ifndef UTILITY_HPP
#define UTILITY_HPP

static const double ray_offset = 0.001;

enum class Command : int 
{
	RENDER,
	SCENEINFO,
	PIXELRAY,
	FIRSTHIT,
	PIXELCOLOR
};

// Utility functions
void parse_optional(int argc, char *argv[], std::vector<unsigned int> &optional);
Command is_valid_command(int argc, char *argv[]);

#endif