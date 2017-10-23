#include "mesh.h"

#include "logger.h"

#include "tiny_obj_loader.h"

#include <fstream>

using namespace graphics;

MeshCache::MeshCache()
{
}

MeshCache::~MeshCache()
{
}

Mesh * MeshCache::GetFromFile(const std::string &name, const std::string & file_path, size_t * hash)
{
	std::string error;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes{};
	std::vector<tinyobj::material_t> materials{};

	size_t name_hash = std::hash<std::string>{}(name);
	if (meshes_.find(name_hash) != meshes_.end())
	{
		if (hash)
		{
			*hash = name_hash;
		}

		return &meshes_[name_hash];
	}

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &error, file_path.c_str()))
	{

		meshes_.insert({ name_hash, Mesh() });

		auto &mesh = meshes_[name_hash];
		mesh.num_meshes = (Uint32)shapes.size();
		mesh.num_indices.reserve(shapes.size());

		std::unordered_map<MeshVertex, Uint32> unique_vertices;
		std::vector<MeshVertex> vertices;

		for (size_t s = 0; s < shapes.size(); ++s)
		{

			auto &shape = shapes[s];

			mesh.num_indices.push_back((Uint32)shape.mesh.indices.size());

			for (auto &index : shape.mesh.indices)
			{
				MeshVertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				if (attrib.normals.size() > 0 && index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}
				
				if (attrib.texcoords.size() > 0 && index.texcoord_index >= 0)
				{
					vertex.texcoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (unique_vertices.count(vertex) == 0)
				{
					unique_vertices[vertex] = (Uint32)vertices.size();
					vertices.push_back(vertex);
				}

				mesh.indices.push_back(unique_vertices[vertex]);
			}
		}

		glGenBuffers(1, &mesh.vbo);
		glGenBuffers(1, &mesh.ebo);
		glGenVertexArrays(1, &mesh.vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Uint32) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		Uint32 attrib_index = 0;

		glBindVertexArray(mesh.vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);

		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

		glEnableVertexAttribArray(attrib_index);
		glVertexAttribPointer(attrib_index, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(MeshVertex), (void*)offsetof(MeshVertex, position));
		glVertexAttribDivisor(attrib_index, 0);
		attrib_index++;

		glEnableVertexAttribArray(attrib_index);
		glVertexAttribPointer(attrib_index, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));
		glVertexAttribDivisor(attrib_index, 0);
		attrib_index++;

		glEnableVertexAttribArray(attrib_index);
		glVertexAttribPointer(attrib_index, 2, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(MeshVertex), (void*)offsetof(MeshVertex, texcoord));
		glVertexAttribDivisor(attrib_index, 0);
		attrib_index++;

		glBindVertexArray(0);

	}
	else
	{
		debug::Log(SDL_LOG_PRIORITY_ERROR, SDL_LOG_CATEGORY_INPUT, error.c_str());
		assert(false && "Could not load mesh from file.");
	}

	if (hash)
	{
		*hash = name_hash;
	}

	return &meshes_[name_hash];
}

Mesh * MeshCache::GetFromHash(size_t hash)
{
	if (meshes_.find(hash) != meshes_.end())
	{
		return &meshes_[hash];
	}

	debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_RENDER, "Mesh was null.");
	return nullptr; 
}

Mesh * MeshCache::GetFromName(const std::string & name)
{
	auto hash = std::hash<std::string>{}(name);
	if (meshes_.find(hash) != meshes_.end())
	{
		return &meshes_[hash];
	}

	debug::Log(SDL_LOG_PRIORITY_CRITICAL, SDL_LOG_CATEGORY_RENDER, "Mesh was null.");
	return nullptr;
}

Mesh * MeshCache::GetFromData(size_t &hash, const std::string &name,
	const std::vector<Uint32> &indices, const std::vector<MeshVertex> &vertices)
{
	hash = std::hash<std::string>{}(name);
	if (meshes_.find(hash) != meshes_.end())
	{
		return &meshes_[hash];
	}

	meshes_.insert({ hash, Mesh() });

	auto &mesh = meshes_[hash];
	mesh.num_meshes = 1;
	mesh.num_indices.push_back((Uint32)indices.size());

	for (auto index : indices)
	{
		mesh.indices.push_back(index);
	}

	glGenBuffers(1, &mesh.vbo);
	glGenBuffers(1, &mesh.ebo);
	glGenVertexArrays(1, &mesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Uint32) * mesh.indices.size(), mesh.indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	Uint32 attrib_index = 0;

	glBindVertexArray(mesh.vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(MeshVertex), (void*)offsetof(MeshVertex, position));
	glVertexAttribDivisor(attrib_index, 0);
	attrib_index++;

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));
	glVertexAttribDivisor(attrib_index, 0);
	attrib_index++;

	glEnableVertexAttribArray(attrib_index);
	glVertexAttribPointer(attrib_index, 2, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(MeshVertex), (void*)offsetof(MeshVertex, texcoord));
	glVertexAttribDivisor(attrib_index, 0);
	attrib_index++;

	glBindVertexArray(0);

	return &meshes_[hash];
}
