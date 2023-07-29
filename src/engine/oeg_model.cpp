#include "oeg_model.h"
#include "oeg_utils.h"

// libs
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <unordered_map>

namespace std{
	template <>
	struct hash<oeg::OegModel::Vertex>
	{
		size_t operator()(oeg::OegModel::Vertex const &vertex) const
		{
			size_t seed = 0;
			oeg::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace oeg {
	OegModel::OegModel(OegDevice& device, const OegModel::Builder& builder)
		:oegDevice(device)
	{
			createVertexBuffer(builder.vertices);
			createIndexBuffer(builder.indices);
	}

	OegModel::~OegModel() = default;

	std::unique_ptr<OegModel> OegModel::createModelFromFile(OegDevice& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		fmt::print("Vertex Count: {}\n", builder.vertices.size());
		return std::make_unique<OegModel>(device, builder);
	}

	void OegModel::createVertexBuffer(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3...");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]); // PLEASE PUT AN INDEX ON THIS OR YOUR COMPUTER WILL HANG

		OegBuffer stagingBuffer
		{
			oegDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void *)vertices.data());

		vertexBuffer = std::make_unique<OegBuffer>(
			oegDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // used to hold vertex input data
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT // host = CPU
			);

		oegDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
		// it will destroy anyway
	}

	void OegModel::createIndexBuffer(const std::vector<uint32_t> &indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if(!hasIndexBuffer)
		{
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		OegBuffer stagingBuffer
		{
			oegDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // used to source location for memory buffer
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT // host = CPU
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<OegBuffer>(
			oegDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // used to hold index input data
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT // host = CPU
		);


		oegDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);

	}

	void OegModel::draw(VkCommandBuffer commandBuffer) 
	{
		if (hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void OegModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = {vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if(hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	std::vector<VkVertexInputBindingDescription> OegModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> OegModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions .push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)
			});
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)
			});
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)
			});
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)
			});
		return attributeDescriptions;
	}

	bool OegModel::Vertex::operator==(const OegModel::Vertex &other) const {
		return position == other.position && color == other.color && normal == other.normal &&
			   uv == other.uv;
	}

	/**
		* Loads a 3D model from the specified file.
		*
		* @param filepath The path to the model file.
		*
		* @throws std::runtime_error if there was an error loading the model.
	*/
	void OegModel::Builder::loadModel(const std::string& filepath)
	{
		// Initialize variables for storing model data
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		// Attempt to load the model using the TinyObjLoader library
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			// Throw a runtime error if there was an error loading the model
			throw std::runtime_error(warn + err);
		}

        fmt::print("IT LOADED\n");
		vertices.clear();
		indices.clear();
		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto &shape : shapes)
		{
			for(const auto &index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}


}
