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

Mesh & MeshCache::GetFromFile(size_t & hash, const std::string &name, const std::string & file_path)
{
	std::string error;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes{};
	std::vector<tinyobj::material_t> materials{};

	if (tinyobj::LoadObj(&attrib, &shapes, &materials, &error, file_path.c_str()))
	{

		hash = std::hash<std::string>{}(name);

		// TODO: Find existing and return preemptively
		meshes_.insert({ hash, Mesh() });
		auto &mesh = meshes_[hash];
		mesh.num_meshes = (Uint32)shapes.size();
		mesh.num_indices.reserve(shapes.size());
		
		//std::vector<float> data;

		std::unordered_map<MeshVertex, Uint32> unique_vertices;
		std::vector<MeshVertex> vertices;

		for (size_t s = 0; s < shapes.size(); ++s)
		{

			auto &shape = shapes[s];

			mesh.num_indices.push_back((Uint32)shape.mesh.indices.size());

			for (auto &index : shape.mesh.indices)
			{
				mesh.indices.push_back((Uint32)mesh.indices.size());

				MeshVertex vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.vertices[3 * index.normal_index + 0],
						attrib.vertices[3 * index.normal_index + 1],
						attrib.vertices[3 * index.normal_index + 2]
					};
				}
				
				if (index.texcoord_index >= 0)
				{
					vertex.texcoord = {
						attrib.vertices[2 * index.texcoord_index + 0],
						attrib.vertices[2 * index.texcoord_index + 1]
					};
				}

				if (unique_vertices.count(vertex) == 0)
				{
					unique_vertices[vertex] = (Uint32)vertices.size();
					vertices.push_back(vertex);
				}
			}

			/*for (size_t f = 0; f < shape.mesh.indices.size() / 3; ++f)
			{
				tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
				tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
				tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

				mesh.indices.push_back(idx0.vertex_index);
				mesh.indices.push_back(idx1.vertex_index);
				mesh.indices.push_back(idx2.vertex_index);

				float texcoords[3][2];
				if (attrib.texcoords.size() > 0)
				{
					assert(attrib.texcoords.size() > 2 * idx0.texcoord_index + 1);
					assert(attrib.texcoords.size() > 2 * idx1.texcoord_index + 1);
					assert(attrib.texcoords.size() > 2 * idx2.texcoord_index + 1);

					// Flip Y coord.
					texcoords[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
					texcoords[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
					texcoords[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
					texcoords[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
					texcoords[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
					texcoords[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
				}
				else
				{
					for (int k = 0; k < 3; ++k)
					{
						texcoords[k][0] = 0.0f;
						texcoords[k][1] = 0.0f;
					}
				}

				float vertices[3][3];
				for (int k = 0; k < 3; ++k)
				{
					int f0 = idx0.vertex_index;
					int f1 = idx1.vertex_index;
					int f2 = idx2.vertex_index;

					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);

					vertices[0][k] = attrib.vertices[3 * f0 + k];
					vertices[1][k] = attrib.vertices[3 * f1 + k];
					vertices[2][k] = attrib.vertices[3 * f2 + k];
				}

				float normals[3][3];
				if (attrib.normals.size() > 0)
				{
					int f0 = idx0.normal_index;
					int f1 = idx1.normal_index;
					int f2 = idx2.normal_index;
					assert(f0 >= 0);
					assert(f1 >= 0);
					assert(f2 >= 0);
					for (int k = 0; k < 3; k++)
					{
						normals[0][k] = attrib.normals[3 * f0 + k];
						normals[1][k] = attrib.normals[3 * f1 + k];
						normals[2][k] = attrib.normals[3 * f2 + k];
					}
				}
				else
				{
					for (int k = 0; k < 3; k++)
					{
						normals[0][k] = 0.0f;
						normals[1][k] = 0.0f;
						normals[2][k] = 0.0f;
					}
				}

				for (int k = 0; k < 3; ++k)
				{
					data.push_back(vertices[k][0]);
					data.push_back(vertices[k][1]);
					data.push_back(vertices[k][2]);
					data.push_back(normals[k][0]);
					data.push_back(normals[k][1]);
					data.push_back(normals[k][2]);
					data.push_back(texcoords[k][0]);
					data.push_back(texcoords[k][1]);
				}

			}*/
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

	return meshes_[hash];
}

Mesh & MeshCache::GetFromHash(size_t hash)
{
	if (meshes_.find(hash) == meshes_.end())
	{
		assert(false);
	}

	return meshes_[hash];
}
