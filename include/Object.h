#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <Mesh.h>
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
    Object()
    {
        std::ifstream ifs("plane.obj");
        std::string str;
        if (ifs.fail())
        {
            std::cerr << "obj load failed!" << std::endl;
        }

		//vertex�ꗗ��index�t���ĂԂ����񂾏�ŁAface�����Ȃ��ƃ_��
		//face���ꗗ����ĂԂ����܂Ȃ��ƃ_������
		std::vector<Vec3> vertices;
        while (std::getline(ifs, str))
        {
            std::vector<std::string> splitted_str = split(&str, ' ');
            if (splitted_str.empty())
            {
                continue;
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
            }
			if (splitted_str[0] == "f")
			{
				//faces
				//����4���_�ȏ�ɂ͔�Ή�.face+3���_��4�v�f
				if (splitted_str.size() > 4)
				{
					continue;
				}
				//make triangle
				std::cout << splitted_str[1] <<", "<< splitted_str[2] << ", " << splitted_str[3] << std::endl;
				// index��v/vt/vn�œ����Ă���̂ŁA0�Ԗڂ����������Ă���B�܂��A���ʂ�1based�Ȃ̂�-1����0based�ɂ���
				int face_vertex_index_1 = std::atoi(split(&splitted_str[1], '/')[0].c_str()) - 1;
				int face_vertex_index_2 = std::atoi(split(&splitted_str[2], '/')[0].c_str()) - 1;
				int face_vertex_index_3 = std::atoi(split(&splitted_str[3], '/')[0].c_str()) - 1;
				_meshs.push_back(new Triangle(vertices[face_vertex_index_1],
											  vertices[face_vertex_index_2],
											  vertices[face_vertex_index_3],
											  new Lambertion(Vec3(1.0, 0.0, 0.0), Vec3())));
			}
            //if (splitted_str[0] == "vn")
            //{
            //    //vertex normal
            //}

        }
    }
    ~Object()
    {
        _meshs.clear();
    };
    std::vector<Mesh *> objects()
    {
        return _meshs;
    }


private:
    BVHTree* _bvh_tree;
    std::vector<Mesh *> _meshs;
};

}//namespace LtRenderer