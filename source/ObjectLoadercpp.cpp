#include <regex>
#include <fstream>
#include <sstream>
#include "ObjectLoader.h"


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

ObjectLoader::ObjectLoader() {}

ObjectLoader::~ObjectLoader() {}

std::map<std::string, Material *> ObjectLoader::_load_materials(std::string material_name)
{
	std::ifstream ifs(material_name);
	std::string str;
	if (ifs.fail())
	{
		std::cerr << "material load failed!" << std::endl;
	}
	std::map<std::string, Material *> name_material_map;

	std::string name;
	Vec3 diffuse;
	Image* texture = nullptr;
	Vec3 emission;
	while (std::getline(ifs, str))
	{
		std::vector<std::string> splitted_str = split(&str, ' ');
		if (splitted_str.empty())
		{
			continue;
		}
		//name
		if (splitted_str[0] == "newmtl")
		{
			name = splitted_str[1];
		}
		//diffuse
		if (splitted_str[0] == "Kd")
		{
			diffuse = Vec3(atof(splitted_str[1].c_str()),
						   atof(splitted_str[2].c_str()),
						   atof(splitted_str[3].c_str()));
		}
		//diffuse��texture���h�����Ă���ꍇ
		if (splitted_str[0] == "map_Kd")
		{
			texture = ImageLoader().load("assets/"+splitted_str[1]);
		}
		//ambient�͎g��Ȃ��̂�emission�Ƃ���map
		if (splitted_str[0] == "Ka")
		{
			emission = Vec3(atof(splitted_str[1].c_str()),
							atof(splitted_str[2].c_str()),
							atof(splitted_str[3].c_str()));
		}

		if (std::regex_search(name, std::regex("dielectric")))
		{
			name_material_map[name] = new Dielectric(diffuse, emission, 1.5);
		}
		else if (std::regex_search(name, std::regex("metal")))
		{
			name_material_map[name] = new Metal(diffuse, emission);
		}
        else
        {
            //default��lambert���g���悤��
            name_material_map[name] = new Lambertion(diffuse, emission);
        }
        if (texture == nullptr)
        {
            continue;
        }
        name_material_map[name]->setAlbedoTexture(texture);
	}
	return name_material_map;
}

std::vector<Mesh *> ObjectLoader::load()
{
	//�����triangle�̂ݑΉ�
	std::vector<Mesh *> triangles;
	std::ifstream ifs("assets/plate_with_texture.obj");
	//std::ifstream ifs("assets/cornell_box.obj");
	if (ifs.fail())
	{
		std::cerr << "obj load failed!" << std::endl;
	}

	std::map<std::string, Material *> name_material_map;
	std::string use_material_name;
	std::vector<Vec3> vertices;
	std::vector<Vec2> vertex_coordinates;
	std::string str;
	while (std::getline(ifs, str))
	{
		std::vector<std::string> splitted_str = split(&str, ' ');
		if (splitted_str.empty())
		{
			continue;
		}
		if (splitted_str[0] == "mtllib")
		{
			name_material_map = _load_materials("assets/"+splitted_str[1]);
		}
		if (splitted_str[0] == "usemtl")
		{
			use_material_name = splitted_str[1];
		}
		if (splitted_str[0] == "v")
		{
			//vertices
			vertices.push_back(Vec3(std::stod(splitted_str[1]),
									std::stod(splitted_str[2]),
									std::stod(splitted_str[3])));

		}
		if (splitted_str[0] == "vt")
		{
			//texture coordinates
            vertex_coordinates.push_back(Vec2(std::stod(splitted_str[1]),
                                              std::stod(splitted_str[2])));
		}
		if (splitted_str[0] == "f")
		{
			//faces
			//����4���_�ȏ�ɂ͔�Ή�.face+3���_��4�v�f
			if (splitted_str.size() > 4)
			{
				continue;
			}
			// index��v/vt/vn�œ����Ă���̂ŁA0�Ԗڂ����������Ă���B�܂��A���ʂ�1based�Ȃ̂�-1����0based�ɂ���
			int face_vertex_index_1 = std::atoi(split(&splitted_str[1], '/')[0].c_str()) - 1;
			int face_vertex_index_2 = std::atoi(split(&splitted_str[2], '/')[0].c_str()) - 1;
			int face_vertex_index_3 = std::atoi(split(&splitted_str[3], '/')[0].c_str()) - 1;
            // uv�̈ʒu����������
			int vertex_coordinate_index_1 = std::atoi(split(&splitted_str[1], '/')[1].c_str()) - 1;
			int vertex_coordinate_index_2 = std::atoi(split(&splitted_str[2], '/')[1].c_str()) - 1;
			int vertex_coordinate_index_3 = std::atoi(split(&splitted_str[3], '/')[1].c_str()) - 1;

			auto vertex_1 = Vertex(vertices[face_vertex_index_1], vertex_coordinates[vertex_coordinate_index_1]);
			auto vertex_2 = Vertex(vertices[face_vertex_index_2], vertex_coordinates[vertex_coordinate_index_2]);
			auto vertex_3 = Vertex(vertices[face_vertex_index_3], vertex_coordinates[vertex_coordinate_index_3]);
			triangles.push_back(new Triangle(vertex_1,
                                             vertex_2,
                                             vertex_3,
											 name_material_map[use_material_name]));
		}
	}
	return triangles;
}

}//namespace LtRenderer