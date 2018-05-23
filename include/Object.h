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

		//vertex一覧をindex付けてぶっこんだ上で、faceを作らないとダメ
		//faceも一覧作ってぶっこまないとダメそう
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
				//現在4頂点以上には非対応.face+3頂点で4要素
				if (splitted_str.size() > 4)
				{
					continue;
				}
				//make triangle
				std::cout << splitted_str[1] <<", "<< splitted_str[2] << ", " << splitted_str[3] << std::endl;
				// indexはv/vt/vnで入っているので、0番目を引っ張ってくる。また、結果は1basedなので-1して0basedにする
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