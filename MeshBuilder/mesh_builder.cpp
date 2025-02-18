#define TINYOBJLOADER_IMPLEMENTATION

#include <Assets/mesh.h>
#include <Reflection/reflection.h>
#include <Logging/logger_service.h>
#include <tiny_obj_loader.h>
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace Engine;
using namespace Extension;

static const char s_ChannelName[] = "RendererModule.MeshBuilder";


static bool LoadObj(const std::string& inPath, std::vector<RendererModule::Assets::Vertex>& outputVertices, std::vector<int>& outputIndices)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials; // TODO: how to load material info?
	std::string warn, err;

	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inPath.c_str());
	
	if (warn.size() > 0)
	{
		Core::Logging::GetLogger()->Warning(s_ChannelName, "Tiny Obj Loader warning: %s", warn.c_str());
	}
	
	if (err.size() > 0)
	{
		Core::Logging::GetLogger()->Error(s_ChannelName, "Tiny Obj Loader error: %s", err.c_str());
	}

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inPath.c_str()))
	{
		Core::Logging::GetLogger()->Error(s_ChannelName, "Obj file failed to load!");
		return false;
	}

	// deduplication
	std::string rollingKeyBuffer;
	rollingKeyBuffer.reserve(64);
	std::unordered_map<std::string, unsigned int> vertexMap;

	for (const tinyobj::shape_t& shape : shapes)
	{
		// load each index of this face into the buffer
		for (const tinyobj::index_t& index : shape.mesh.indices)
		{
			// attempt deduplication
			{
				rollingKeyBuffer.clear();
				rollingKeyBuffer.append(std::to_string(index.vertex_index));
				rollingKeyBuffer.append(std::to_string(index.normal_index));
				rollingKeyBuffer.append(std::to_string(index.texcoord_index));

				auto foundIndex = vertexMap.find(rollingKeyBuffer);
				if (foundIndex != vertexMap.end())
				{
					outputIndices.push_back(foundIndex->second);
					continue;
				}
				else
				{
					vertexMap[rollingKeyBuffer] = (unsigned int)outputVertices.size();
				}
			}

			RendererModule::Assets::Vertex vertex;

			if (index.vertex_index >= 0)
			{
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
			}

			if (index.normal_index >= 0)
			{
				vertex.normal = {
					attrib.normals[3 * index.normal_index +0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (index.texcoord_index >= 0)
			{
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			outputVertices.push_back(vertex);
			outputIndices.push_back((unsigned int)outputVertices.size() - 1);
		}
	}

	return true;
}

int main()
{
	std::vector<RendererModule::Assets::Vertex> outputVertices;
	std::vector<int> outputIndices;
	
	// read in vertices and indices
	if (!LoadObj("C:\\users\\wonjo\\Desktop\\cube.obj", outputVertices, outputIndices))
	{
		return 0b1;
	}

	// write them out
	std::ofstream outputFile("C:\\users\\wonjo\\Desktop\\cube.se_mesh");
	
	unsigned int vertexCount = (unsigned int)outputVertices.size();
	unsigned int indexCount = (unsigned int)outputIndices.size();
	outputFile
		.write((const char*)&vertexCount, sizeof(vertexCount))
		.write((const char*)(outputVertices.data()), outputVertices.size() * sizeof(RendererModule::Assets::Vertex))
		.write((const char*)&indexCount, sizeof(indexCount))
		.write((const char*)(outputIndices.data()), outputIndices.size() * sizeof(int));
}
