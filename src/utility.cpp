#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "utility.hpp"

void parse_optional(int argc, char *argv[], std::vector<unsigned int> &optional)
{
	unsigned int temp;
	for (int i = 3; i < argc; i++)
	{
		// TODO(kjayakum): Wrap the ss >> temp in an if guard for string with '-' in it
		std::stringstream ss(argv[i]);
		ss >> temp;
		optional.push_back(temp);
	}
}

Command is_valid_command(int argc, char *argv[])
{
	std::string cur;
	std::ifstream test_file;
	Command type;

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
	else if(cur == "pixelcolor")
		type = Command::PIXELCOLOR;
	else
	{
		std::cerr << "Invalid command entered" << std::endl;
		exit(EXIT_FAILURE);
	}

	cur = std::string(argv[2]);
	if(cur.find(".pov") == std::string::npos)
	{
		std::cerr << "Invalid input file: Only read in .pov files" << std::endl;
		exit(EXIT_FAILURE);
	}
	test_file.open(cur);
	if(!test_file.good())
	{
		perror(cur.c_str());
		exit(EXIT_FAILURE);
	}
	test_file.close();


	return type;
}