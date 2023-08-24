#pragma once
#include<memory>
#include"mesh.hpp"
#include"uv.hpp"
#include"transform.hpp"

#include<map>


class Object;
std::vector<Object>objs;

class Object {
public:
	std::shared_ptr<Device>device;
	const char* mesh;
	int textureindex;
	const char* texture;
	Transform transform;
	Object(std::shared_ptr<Device>device, const char* mesh, const char* texture) :device{ device }, mesh{ mesh }, texture{ texture }{
		bool existM = false;
		bool existU = false;

		for (auto& kv : meshs) {
			if (kv.first == this->mesh) {
				existM = true;
				break;
			}
		}

		if (!existM) {
			
			Mesh* new_mesh = new Mesh(device, this->mesh);
			meshs.insert(std::pair<const char*, std::shared_ptr<Mesh>>(this->mesh, std::shared_ptr<Mesh>(new_mesh)));


		}

		int index = 0;
		for (auto& kv : textures) {
			if (kv.first == this->texture) {
				existU = true;
				textureindex = index;
				break;
			}
			index++;
		}
		
		if (!existU) {

			Texture* new_texture = new Texture(device, this->texture);
			
			textures.insert(std::pair<const char*, std::shared_ptr<Texture>>(this->texture, std::shared_ptr<Texture>(new_texture)));
			textureindex = textures.size() - 1;

		}
	}
	void ready() {
		objs.push_back(*this);
	}

	

};