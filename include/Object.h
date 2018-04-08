#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <BVH.h>

namespace LtRenderer
{

std::vector<std::string> split(const std::string *str, char delimiter)
{
	std::vector<std::string> splitted_str;
	std::stringstream str_stream(*str);
	std::string tmp_buffer;
	while (std::getline(str_stream, tmp_buffer, delimiter))
	{
		splitted_str.push_back(tmp_buffer);
	}
	return splitted_str;
}

class Object {
public:
	//Object() {};
	//Object(std::string file_path)
	Object()
	{
		std::ifstream ifs("cube.obj");
		std::string str;
		if (ifs.fail())
		{
			std::cerr << "obj load failed!" << std::endl;
		}
		
		while (std::getline(ifs, str))
		{
			std::vector<std::string> splitted_str = split(&str, ' ');
			if (str[0] == 'v')
			{
				std::cout << str << ", true" << std::endl;
			}
			else
			{
				std::cout << str << ", false because => " << str[0] << std::endl;
			}
			//str.substr Šo‚¦‚Æ‚­‚Æ—Ç‚³‚°
			//for (auto hoge : str)
			//{
			//	std::cout << hoge << std::endl;
			//}
		}
	}
	~Object() {};

private:
	BVHTree* bvh_tree;
};

}//namespace LtRenderer