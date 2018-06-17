#pragma once
#include <map>
#include <string>
#include "Mesh.h"
#include "Material.h"
#include "Image.h"

namespace LtRenderer
{

class ObjectLoader {
public:
	ObjectLoader();
    ~ObjectLoader();
	std::map<std::string, Material *> _load_materials(std::string material_name);
	std::vector<Mesh *> load();
};

}//namespace LtRenderer