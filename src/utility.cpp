#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include "utility.hpp"

void parse_optional(int argc, char *argv[], std::vector<unsigned int> &optional)
{
	unsigned int temp;
	for (int i = 3; i < argc; i++)
	{
		std::stringstream ss(argv[i]);
		ss >> temp;
		if((ss.rdstate() & std::stringstream::failbit) == 0)
		{
			optional.push_back(temp);
		}
	}
}

uint get_supersample(int argc, char *argv[])
{
	std::string ss_flag = "-ss=";
	char *ss_num;
	uint sample_size = 1;

	for(int i = 3; i < argc; i++)
	{
		ss_num = strstr(argv[i], ss_flag.c_str());
		if(ss_num)
		{
			ss_num += 4;
			sample_size = strtoul(ss_num, NULL, 0);
			break;
		}
	}
	return sample_size;
}

void get_flags(int argc, char *argv[], std::array<bool, 3> &flags)
{
	std::string temp;
	for(int i = 3; i < argc; i++)
	{
		std::stringstream ss(argv[i]);
		ss >> temp;
		if(temp == "-fresnel")
		{
			flags[(int)Flags::FRESNEL] = true;
		}
		else if(temp == "-sds")
		{
			flags[(int)Flags::SPACIAL_DATA_STRUCTURES] = true;
		}
		else if(temp == "-altbrdf")
		{
			flags[(int)Flags::ALTERNATIVE_BRDF] = true;
		}
	}
}

Command is_valid_command(int argc, char *argv[], std::vector<uint> &options)
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

	cur = std::string(argv[1]);
	if(cur == "render")
		type = Command::RENDER;
	else if(cur == "sceneinfo")
		type = Command::SCENEINFO;
	else if(cur == "pixelray")
	{
		type = Command::PIXELRAY;
	}
	else if(cur == "firsthit")
	{
		type = Command::FIRSTHIT;
	}
	else if(cur == "pixelcolor")
	{
		type = Command::PIXELCOLOR;
	}
	else if(cur == "printrays")
	{
		type = Command::PRINTRAYS;
	}
	else
	{
		std::cerr << "Invalid command entered" << std::endl;
		exit(EXIT_FAILURE);
	}

	switch(type)
	{
		case Command::FIRSTHIT:
		case Command::PIXELRAY:
		case Command::PRINTRAYS:
		case Command::PIXELTRACE:
		case Command::PIXELCOLOR:
			if(options.size() < 4)
			{
				std::cerr << "Not enough arguments passed for specified command!" << std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		case Command::RENDER:
		case Command::SCENEINFO:
			break;
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